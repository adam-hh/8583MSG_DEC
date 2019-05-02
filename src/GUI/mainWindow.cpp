#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "interfaceDialog.h"
#include "8583dump.h"
#include "8583.h"
#include "appendThread.h"
#include <pthread.h>
#include <QtCore/QDebug>
#include <QtTest/QTest>

QVector<tcpDataBlock> vec; //store captured data
pthread_t pthLoop, pthRead; //threads
void *threadLoop(void* arg)
{
    DEC::MainWindow *um = static_cast<DEC::MainWindow*>(arg);
    loop(0, callbackWriteToBuff);
    pthread_testcancel();
    return NULL;
}
void *threadReadData(void *arg)
{
    DEC::MainWindow *um = static_cast<DEC::MainWindow*>(arg);
    for(QVector<tcpDataBlock>::iterator it = vec.begin(); it != vec.end(); it++){
        free((*it).data);
    }
    QVector<tcpDataBlock>().swap(vec); //empty vec
    emit um->newData(NULL); // empty treeWidget
    tcpDataBlock b = {0};    
    int cnt = 0;
    int rlt = 0;
    while(true){
        rlt = readFromUserBuff(usbf, &b);
        if(1 == rlt){
            vec.append(b);
            QTreeWidgetItem *item = new QTreeWidgetItem();
            item->setText(0, QString::asprintf("%d", cnt));
            item->setText(1, b.extraInfo);
            item->setText(2, QString::asprintf("%d.%d.%d.%d:%d",
                b.ipSrc.byte1,
                b.ipSrc.byte2,
                b.ipSrc.byte3,
                b.ipSrc.byte4,
                b.portSrc));
            item->setText(3, QString::asprintf("%d.%d.%d.%d:%d",
                b.ipDst.byte1,
                b.ipDst.byte2,
                b.ipDst.byte3,
                b.ipDst.byte4,
                b.portDst));
            item->setText(4, "TCP");
            item->setText(5, b.extraInfo);
            item->setText(6, b.extraInfo);
            emit um->newData(item);
            cnt++;
        }else{
            Sleep(100);
        }
        pthread_testcancel();
    }
}

DEC::MainWindow::MainWindow() :
    ui(new Ui::MainWindow),
    itf(new DEC::interfaceDialog(this))
{
    ui->setupUi(this);
    QRegExp regExpFilter("tcp\\sport(\\s[0-9]{1,5})+");
    ui->lineEdit->setValidator(new QRegExpValidator(regExpFilter, this)); //set Filter rule
    QRegExp regExpTPDU("[0-9]{10}");
    ui->lineEdit_2->setValidator(new QRegExpValidator(regExpTPDU, this)); //set TPDU rule

    ui->textBrowser->setLineWrapMode(QTextEdit::FixedColumnWidth); //
    ui->textBrowser->setLineWrapColumnOrWidth(95); // warp each 32 bytes

    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(findInterface()));
    connect(this, SIGNAL(interfaceScaned(bool)), this, SLOT(enabltPbt4(bool)));
    connect(this, SIGNAL(newData(QTreeWidgetItem*)), this, SLOT(appendNewData(QTreeWidgetItem*)), Qt::DirectConnection);


    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(loop()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(stop()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(restart()));
    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(expandData()));

    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(decodeMsg()));
    connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(decodeMsg()));
    connect(ui->pushButton_9, SIGNAL(clicked()), this, SLOT(decodeMsgManual()));
    connect(ui->pushButton_5, SIGNAL(clicked()), this, SLOT(testTPDU()));
    connect(ui->pushButton_6, SIGNAL(clicked()), this, SLOT(trimTextEdit()));
    connect(ui->pushButton_7, SIGNAL(clicked()), this, SLOT(alignTextEdit()));
    connect(ui->pushButton_8, SIGNAL(clicked()), this, SLOT(clearTextEdit()));
}

DEC::MainWindow::~MainWindow()
{
    delete ui;
    delete itf;
}

void DEC::MainWindow::findInterface()
{
    itf->setModal(true);
    itf->show();
    itf->showDevList();
}

void DEC::MainWindow::enabltPbt4(bool bl)
{
    ui->pushButton_4->setEnabled(bl);
}
void DEC::MainWindow::setPbt4(bool bl)
{
    emit interfaceScaned(bl);
}
void DEC::MainWindow::setPbt(bool bl)
{
    ui->pushButton->setEnabled(bl);
}
int DEC::MainWindow::appendNewData(QTreeWidgetItem *item)
{
    QMutex mu;
    mu.lock();
    if(NULL == item){
        ui->treeWidget->clear();
        return 0;
    }
    else{
        ui->treeWidget->addTopLevelItem(item);
        ui->treeWidget->verticalScrollBar()->setValue(ui->treeWidget->verticalScrollBar()->maximum());
        //ui->treeWidget->scrollToBottom();  //crashing
    }
    mu.unlock();
    return 1;
}
int DEC::MainWindow::loop()
{
    if(!(ui->lineEdit->hasAcceptableInput())){
        QMessageBox::information(this, "Title", "Reg mismatch.");
        return -1;
    }
    if(1 != setFilter(ui->lineEdit->text().toLocal8Bit().data())){
        QMessageBox::information(this, "Title", "Set filter failed.");
        return -1;
    }
    if (pthread_create(&pthLoop, NULL, threadLoop, static_cast<void *>(this))) {
        QMessageBox::information(this, "Title", "Start pthLoop failed.");
        return -1;
    }
    if (pthread_create(&pthRead, NULL, threadReadData, static_cast<void *>(this))) {
        QMessageBox::information(this, "Title", "Start pthLoop failed.");
        return -1;
    }
    ui->pushButton->setEnabled(false);
    ui->pushButton_4->setEnabled(false);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    return 1;
}
int DEC::MainWindow::expandData()
{
    int in = ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->currentItem());
    if(in < vec.size()){        
        QByteArray qB2 = QByteArray::fromRawData((char*)vec[in].data, vec[in].dataLen).toHex(0x20);
        ui->textBrowser->setPlainText(QString::fromUtf8(qB2).toUpper());
    }
    return 1;
}
int DEC::MainWindow::decode()
{
    return 1;
}
int DEC::MainWindow::stop()
{
    pcap_breakloop(PHandle.handle);
    pthread_cancel(pthLoop);
    pthread_cancel(pthRead);
    if(pthread_join(pthLoop, NULL)){
        QMessageBox::information(this, "Title", "Error joining thread pthLoop.");
        return -1;
    }
    if(pthread_join(pthRead, NULL)){
        QMessageBox::information(this, "Title", "Error joining thread pthRead.");
        return -1;
    }
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->pushButton->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    return 1;
}
int DEC::MainWindow::restart()
{
    if(-1 == DEC::MainWindow::stop()){
        QMessageBox::information(this, "Title", "Error stop.");
        return -1;
    }
    if(-1 == DEC::MainWindow::loop()){
        QMessageBox::information(this, "Title", "Error restart.");
        return -1;
    }
    ui->pushButton_4->setEnabled(false);
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(true);
    ui->textBrowser->clear();
    return 1;
}
int DEC::MainWindow::decodeMsg()
{
    ui->textBrowser_2->setPlainText(NULL);
    ui->tableWidget->clearContents();
    int in = ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->currentItem());
    //ui->treeWidget->currentItem()->setBackgroundColor(0, QColor("#0000FF"));
    if(in < vec.size()){
        u32 msg8583Len = 0;
        u8* msg8583 = ::testTPDU("6000010000", vec[in].data, vec[in].dataLen, &msg8583Len);
        if(NULL != msg8583){
            MsgJL msgjl = {0};
            initConsoleBuf();
            if(OK == DecodeJLMsg(msg8583, msg8583Len, &msgjl)){
                //QMessageBox::information(this, "Title", "decodeMsgManual sucess.");// decode sucess
                QByteArray tmp = QByteArray::fromRawData((char*)msgjl.MsgLen, sizeof(msgjl.MsgLen)).toHex();
                ui->tableWidget->setItem(0, 0, new QTableWidgetItem("MsgLen"));
                ui->tableWidget->setItem(0, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));

                tmp = QByteArray::fromRawData((char*)msgjl.MsgHead, sizeof(msgjl.MsgHead)).toHex();
                ui->tableWidget->setItem(1, 0, new QTableWidgetItem("MsgHead"));
                ui->tableWidget->setItem(1, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));

                tmp = QByteArray::fromRawData((char*)msgjl.MsgTPDU, sizeof(msgjl.MsgTPDU)).toHex();
                ui->tableWidget->setItem(2, 0, new QTableWidgetItem("MsgTPDU"));
                ui->tableWidget->setItem(2, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));

                tmp = QByteArray::fromRawData((char*)msgjl.Field0, sizeof(msgjl.Field0)).toHex();
                ui->tableWidget->setItem(3, 0, new QTableWidgetItem("Field0"));
                ui->tableWidget->setItem(3, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));

                #define F(x,y) \
                if(0 != msgjl.x##_l){ \
                    tmp = QByteArray::fromRawData((char*)msgjl.x, msgjl.x##_l).toHex();\
                    ui->tableWidget->setItem(y, 0, new QTableWidgetItem(#x));\
                    ui->tableWidget->setItem(y, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));\
                }else{\
                    ui->tableWidget->setItem(y, 0, new QTableWidgetItem(#x));\
                    ui->tableWidget->setItem(y, 1, new QTableWidgetItem("Empty"));\
                }
                F(Field1, 4) F(Field2, 5) F(Field3, 6) F(Field4, 7) F(Field5, 8) F(Field6, 9) F(Field7, 10) F(Field8, 11) F(Field9, 12) F(Field10, 13)
                F(Field11, 14) F(Field12, 15) F(Field13, 16) F(Field14, 17) F(Field15, 18) F(Field16, 19) F(Field17, 20) F(Field18, 21) F(Field19, 22) F(Field20, 23)
                F(Field21, 24) F(Field22, 25) F(Field23, 26) F(Field24, 27) F(Field25, 28) F(Field26, 29) F(Field27, 30) F(Field28, 31) F(Field29, 32) F(Field30, 33)
                F(Field31, 34) F(Field32, 35) F(Field33, 36) F(Field34, 37) F(Field35, 38) F(Field36, 39) F(Field37, 40) F(Field38, 41) F(Field39, 42) F(Field40, 43)
                F(Field41, 44) F(Field42, 45) F(Field43, 46) F(Field44, 47) F(Field45, 48) F(Field46, 49) F(Field47, 50) F(Field48, 51) F(Field49, 52) F(Field50, 53)
                F(Field51, 54) F(Field52, 55) F(Field53, 56) F(Field54, 57) F(Field55, 58) F(Field56, 59) F(Field57, 60) F(Field58, 61) F(Field59, 62) F(Field60, 63)
                F(Field61, 64) F(Field62, 65) F(Field62, 66) F(Field63, 67) F(Field64, 68)
                #undef F
                ui->treeWidget->currentItem()->setBackgroundColor(0, QColor("#00FF00"));
            }else{
                //QMessageBox::information(this, "Title", "decodeMsgManual(illegal field) fail.");
                ui->treeWidget->currentItem()->setBackgroundColor(0, QColor("#FF0000"));
                return -1;
            }
            ui->textBrowser_2->setPlainText(QString::asprintf("%s", consolebuffer.buf));
            clearConsoleBuf();
        }else{
            //QMessageBox::information(this, "Title", "decodeMsgManual(TPDU test fail) fail.");
            ui->treeWidget->currentItem()->setBackgroundColor(0, QColor("#FF0000"));
            return -1;
        }
    }
    return 1;
}
int DEC::MainWindow::decodeMsgManual()
{
    ui->textBrowser_2->setPlainText(NULL);
    ui->tableWidget->clearContents();
    if(!(ui->lineEdit_2->hasAcceptableInput())){
        QMessageBox::information(this, "Title", "TPDU mismatch.");
        return -1;
    }
    s8 tpdu[100] = {0};
    strcpy(tpdu, ui->lineEdit_2->text().toLocal8Bit().data());
    s8 msgInput[1000] = {0};
    strcpy(msgInput, ui->textEdit->toPlainText().toLocal8Bit().data());
    u32 len = strlen(msgInput);
    if(len < 20){
        QMessageBox::information(this, "Title", "decodeMsgManual(msglen) fail.");
        return -1;
    }
    u8* msg = (u8*)malloc(len / 2);
    if(!(1 == HexcharStringRevert(msgInput, msg, len / 2))){
        QMessageBox::information(this, "Title", "decodeMsgManual(illegal hex string) fail.");
        return -1;
    }
    u32 msg8583Len = 0;
    u8* msg8583 = ::testTPDU(tpdu, msg, len / 2, &msg8583Len);
    if(NULL != msg8583){
        MsgJL msgjl = {0};
        initConsoleBuf();
        if(OK == DecodeJLMsg(msg8583, msg8583Len, &msgjl)){
            //QMessageBox::information(this, "Title", "decodeMsgManual sucess.");// decode sucess
            QByteArray tmp = QByteArray::fromRawData((char*)msgjl.MsgLen, sizeof(msgjl.MsgLen)).toHex();
            ui->tableWidget->setItem(0, 0, new QTableWidgetItem("MsgLen"));
            ui->tableWidget->setItem(0, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));

            tmp = QByteArray::fromRawData((char*)msgjl.MsgHead, sizeof(msgjl.MsgHead)).toHex();
            ui->tableWidget->setItem(1, 0, new QTableWidgetItem("MsgHead"));
            ui->tableWidget->setItem(1, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));

            tmp = QByteArray::fromRawData((char*)msgjl.MsgTPDU, sizeof(msgjl.MsgTPDU)).toHex();
            ui->tableWidget->setItem(2, 0, new QTableWidgetItem("MsgTPDU"));
            ui->tableWidget->setItem(2, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));

            tmp = QByteArray::fromRawData((char*)msgjl.Field0, sizeof(msgjl.Field0)).toHex();
            ui->tableWidget->setItem(3, 0, new QTableWidgetItem("Field0"));
            ui->tableWidget->setItem(3, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));

            #define F(x,y) \
            if(0 != msgjl.x##_l){ \
                tmp = QByteArray::fromRawData((char*)msgjl.x, msgjl.x##_l).toHex();\
                ui->tableWidget->setItem(y, 0, new QTableWidgetItem(#x));\
                ui->tableWidget->setItem(y, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));\
            }else{\
                ui->tableWidget->setItem(y, 0, new QTableWidgetItem(#x));\
                ui->tableWidget->setItem(y, 1, new QTableWidgetItem("Empty"));\
            }
            F(Field1, 4) F(Field2, 5) F(Field3, 6) F(Field4, 7) F(Field5, 8) F(Field6, 9) F(Field7, 10) F(Field8, 11) F(Field9, 12) F(Field10, 13)
            F(Field11, 14) F(Field12, 15) F(Field13, 16) F(Field14, 17) F(Field15, 18) F(Field16, 19) F(Field17, 20) F(Field18, 21) F(Field19, 22) F(Field20, 23)
            F(Field21, 24) F(Field22, 25) F(Field23, 26) F(Field24, 27) F(Field25, 28) F(Field26, 29) F(Field27, 30) F(Field28, 31) F(Field29, 32) F(Field30, 33)
            F(Field31, 34) F(Field32, 35) F(Field33, 36) F(Field34, 37) F(Field35, 38) F(Field36, 39) F(Field37, 40) F(Field38, 41) F(Field39, 42) F(Field40, 43)
            F(Field41, 44) F(Field42, 45) F(Field43, 46) F(Field44, 47) F(Field45, 48) F(Field46, 49) F(Field47, 50) F(Field48, 51) F(Field49, 52) F(Field50, 53)
            F(Field51, 54) F(Field52, 55) F(Field53, 56) F(Field54, 57) F(Field55, 58) F(Field56, 59) F(Field57, 60) F(Field58, 61) F(Field59, 62) F(Field60, 63)
            F(Field61, 64) F(Field62, 65) F(Field62, 66) F(Field63, 67) F(Field64, 68)
            #undef F
        }
        else{
             QMessageBox::information(this, "Title", "decodeMsgManual(illegal field) fail.");
             return -1;
        }
        ui->textBrowser_2->setPlainText(QString::asprintf("%s", consolebuffer.buf));
        clearConsoleBuf();
    }else{
        QMessageBox::information(this, "Title", "decodeMsgManual(TPDU test fail) fail.");
        return -1;
    }
    return 1;
}
int DEC::MainWindow::testTPDU()//test TPDU
{
    s8 tpdu[TMPMSGSIZE] = {0};
    strcpy(tpdu, ui->lineEdit_2->text().toLocal8Bit().data());
    s8 msgInput[MAXMSGSIZE] = {0};
    strcpy(msgInput, ui->textEdit->toPlainText().toLocal8Bit().data());
    u32 len = strlen(msgInput);
    if(len < 11){
        QMessageBox::information(this, "Title", "test TPDU fail.");
        return -1;
    }
    u8* msg = (u8*)malloc(len / 2);
    if(!(1 == HexcharStringRevert(msgInput, msg, len / 2))){
        QMessageBox::information(this, "Title", "test TPDU(illegal hex string) fail.");
        return -1;
    }
    u32 msg8583Len = 0;
    u8* msg8583 = ::testTPDU(tpdu, msg, len / 2, &msg8583Len);
    if(NULL != msg8583){
        QMessageBox::information(this, "Title", "test TPDU sucess.");
    }else{
        QMessageBox::information(this, "Title", "test TPDU fail.");
        return -1;
    }
    return 1;
}
int DEC::MainWindow::trimTextEdit()//trim(remove space)
{
    s8 msgInput[MAXMSGSIZE] = {0};
    strcpy(msgInput, ui->textEdit->toPlainText().toLocal8Bit().data());
    s8 msgInputNew[MAXMSGSIZE] = {0};
    s8 *msgInputNewPtr = msgInputNew;
    for(s8 *tmp = msgInput; *tmp != 0; tmp++){
        if(*tmp == '\t' || *tmp == '\r' || *tmp == '\n' || *tmp == '\v' || *tmp == '\f' || *tmp == 0x20)
            continue;
        else{
            *msgInputNewPtr = *tmp;
            msgInputNewPtr++;
        }
    }
    ui->textEdit->clear();
    ui->textEdit->setText(QString::asprintf("%s", msgInputNew));
    return 1;
}
int DEC::MainWindow::alignTextEdit()//align(add space)
{
    s8 msgInput[MAXMSGSIZE] = {0};
    strcpy(msgInput, ui->textEdit->toPlainText().toLocal8Bit().data());
    s8 msgInputNew[MAXMSGSIZE * 2] = {0};
    s8 *msgInputNewPtr = &msgInputNew[0];
    for(s8 *tmp = msgInput; *tmp != 0; tmp++){
        if(*tmp == '\t' || *tmp == '\r' || *tmp == '\n' || *tmp == '\v' || *tmp == '\f' || *tmp == 0x20)
            continue;
        else{
            *msgInputNewPtr = *tmp;
            msgInputNewPtr++;
            if(0 == (msgInputNewPtr - &msgInputNew[0] + 1) % 3){
                *msgInputNewPtr = 0x20;
                msgInputNewPtr++;
            }
        }
    }
    ui->textEdit->clear();
    ui->textEdit->setText(QString::asprintf("%s", msgInputNew));
    return 1;
}
int DEC::MainWindow::clearTextEdit()//clear
{
    ui->textEdit->clear();
    return 1;
}