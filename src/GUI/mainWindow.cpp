#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "interfaceDialog.h"
#include "8583dump.h"
#include "8583.h"
#include "ThreadTreeView.h"
#include <pthread.h>
#include <QtCore/QList>

QVector<TcpDataBlock*> DEC::vec; //raw data source
QMutex DEC::mutex; //mutex
pthread_t PthLoop, PthRead; //threads
void *ThreadLoop(void* arg)
{
    DEC::MainWindow *um = static_cast<DEC::MainWindow*>(arg);
    Loop(0, CallbackWriteToBuff);
    pthread_testcancel();
    return NULL;
}
void *ThreadReadData(void *arg)
{
    DEC::MainWindow *um = static_cast<DEC::MainWindow*>(arg);
    foreach(TcpDataBlock* it, DEC::vec){
        free(it->data);
        free(it);
    }
    //QVector<TcpDataBlock*>().swap(vec); //empty vec
    DEC::vec.resize(0);
    um->model->resetModel(); //empty model
    while(true){
        TcpDataBlock *b = (TcpDataBlock*)malloc(sizeof(TcpDataBlock));
        if(1 == ReadFromUserBuff(Usbf, b)){
            DEC::vec.append(b); //store to raw data source
            um->model->appendItem(b);//append to model(then show on GUI)
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
    myTimeId = 0; //timer control auto scroll
    treeViewUpdataFlag = 0;//work with timer
    QRegExp regExpFilter("tcp\\sport(\\s[0-9]{1,5})+");
    ui->lineEdit->setValidator(new QRegExpValidator(regExpFilter, this)); //set Filter rule
    QRegExp regExpTPDU("[0-9]{10}");
    ui->lineEdit_2->setValidator(new QRegExpValidator(regExpTPDU, this)); //set TPDU rule
    ui->textBrowser->setLineWrapMode(QTextEdit::FixedColumnWidth); //
    ui->textBrowser->setLineWrapColumnOrWidth(95); // warp each 32 bytes

    QStringList treeviewHeaders;  //QTreeView headers
    treeviewHeaders << QStringLiteral("No.")
                    << QStringLiteral("Time")
                    << QStringLiteral("Source")
                    << QStringLiteral("Destination")
                    << QStringLiteral("Protocal")
                    << QStringLiteral("Length")
                    << QStringLiteral("Info");
    model  = new dataItemModel(treeviewHeaders);
    model->resetModel();//must reset here
    ui->treeView->setModel(model);

    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(findInterface()));

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(loop()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(stop()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(restart()));
    connect(ui->treeView, SIGNAL(pressed(QModelIndex)), this, SLOT(expandData(QModelIndex)));
    connect(ui->treeView, SIGNAL(pressed(QModelIndex)), this, SLOT(decodeMsg(QModelIndex)));
    connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(selectDecoder(int)));

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
    delete model;
}

void DEC::MainWindow::findInterface()
{
    itf->setModal(true);
    itf->show();
    itf->showDevList();
}
void DEC::MainWindow::setPbt(bool bl)
{
    ui->pushButton->setEnabled(bl);
}
int DEC::MainWindow::loop()
{
    Usbf = InitUserBuff(100000);
    if(!(ui->lineEdit->hasAcceptableInput())){
        QMessageBox::information(this, "Title", "Reg mismatch.");
        return -1;
    }
    if(1 != SetFilter(ui->lineEdit->text().toLocal8Bit().data())){
        QMessageBox::information(this, "Title", "Set filter failed.");
        return -1;
    }
    if (pthread_create(&PthLoop, NULL, ThreadLoop, static_cast<void *>(this))) {
        QMessageBox::information(this, "Title", "Start pthLoop failed.");
        return -1;
    }
    if (pthread_create(&PthRead, NULL, ThreadReadData, static_cast<void *>(this))) {
        QMessageBox::information(this, "Title", "Start pthLoop failed.");
        return -1;
    }
    ui->pushButton->setEnabled(false);
    ui->pushButton_4->setEnabled(false);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    return 1;
}
int DEC::MainWindow::expandData(const QModelIndex& index)
{
    DEC::dataItem *currentItem = static_cast<DEC::dataItem*>(index.internalPointer());
    if(currentItem != NULL){
        QByteArray qB2 = QByteArray::fromRawData((char*)currentItem->tcpData()->data, currentItem->tcpData()->dataLen).toHex(0x20);
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
    pthread_cancel(PthLoop);
    pthread_cancel(PthRead);
    if(pthread_join(PthLoop, NULL)){
        QMessageBox::information(this, "Title", "Error joining thread pthLoop.");
        return -1;
    }
    if(pthread_join(PthRead, NULL)){
        QMessageBox::information(this, "Title", "Error joining thread pthRead.");
        return -1;
    }
    ReleaseUserBuff(Usbf);
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
void DEC::MainWindow::fillTable(void* ptr){
    switch(model->customerid){
        case CUSTOMER_JL:{
            MsgJL* msg = static_cast<MsgJL*>(ptr);
            QByteArray tmp = QByteArray::fromRawData((char*)msg->msgLen, sizeof(msg->msgLen)).toHex();
            ui->tableWidget->setItem(0, 0, new QTableWidgetItem("msgLen"));
            ui->tableWidget->setItem(0, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));

            tmp = QByteArray::fromRawData((char*)msg->msgTPDU, sizeof(msg->msgTPDU)).toHex();
            ui->tableWidget->setItem(1, 0, new QTableWidgetItem("msgTPDU"));
            ui->tableWidget->setItem(1, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));

			tmp = QByteArray::fromRawData((char*)msg->msgHead, sizeof(msg->msgHead)).toHex();
            ui->tableWidget->setItem(2, 0, new QTableWidgetItem("msgHead"));
            ui->tableWidget->setItem(2, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));

            tmp = QByteArray::fromRawData((char*)msg->field0, sizeof(msg->field0)).toHex();
            ui->tableWidget->setItem(3, 0, new QTableWidgetItem("field0"));
            ui->tableWidget->setItem(3, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));

            #define F(x,y) \
            if(0 != msg->x##_l){ \
                tmp = QByteArray::fromRawData((char*)msg->x, msg->x##_l).toHex();\
                ui->tableWidget->setItem(y, 0, new QTableWidgetItem(#x));\
                ui->tableWidget->setItem(y, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));\
            }else{\
                ui->tableWidget->setItem(y, 0, new QTableWidgetItem(#x));\
                ui->tableWidget->setItem(y, 1, new QTableWidgetItem("empty"));\
            }
            F(field1, 4) F(field2, 5) F(field3, 6) F(field4, 7) F(field5, 8) F(field6, 9) F(field7, 10) F(field8, 11) F(field9, 12) F(field10, 13)
            F(field11, 14) F(field12, 15) F(field13, 16) F(field14, 17) F(field15, 18) F(field16, 19) F(field17, 20) F(field18, 21) F(field19, 22) F(field20, 23)
            F(field21, 24) F(field22, 25) F(field23, 26) F(field24, 27) F(field25, 28) F(field26, 29) F(field27, 30) F(field28, 31) F(field29, 32) F(field30, 33)
            F(field31, 34) F(field32, 35) F(field33, 36) F(field34, 37) F(field35, 38) F(field36, 39) F(field37, 40) F(field38, 41) F(field39, 42) F(field40, 43)
            F(field41, 44) F(field42, 45) F(field43, 46) F(field44, 47) F(field45, 48) F(field46, 49) F(field47, 50) F(field48, 51) F(field49, 52) F(field50, 53)
            F(field51, 54) F(field52, 55) F(field53, 56) F(field54, 57) F(field55, 58) F(field56, 59) F(field57, 60) F(field58, 61) F(field59, 62) F(field60, 63)
            F(field61, 64) F(field62, 65) F(field62, 66) F(field63, 67) F(field64, 68)
            #undef F
            break;
        }
        case CUSTOMER_YS:{
            MsgYS* msg = static_cast<MsgYS*>(ptr);
            QByteArray tmp = QByteArray::fromRawData((char*)msg->msgLen, sizeof(msg->msgLen)).toHex();
            ui->tableWidget->setItem(0, 0, new QTableWidgetItem("msgLen"));
            ui->tableWidget->setItem(0, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));

            tmp = QByteArray::fromRawData((char*)msg->msgTPDU, sizeof(msg->msgTPDU)).toHex();
            ui->tableWidget->setItem(1, 0, new QTableWidgetItem("msgTPDU"));
            ui->tableWidget->setItem(1, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));

			tmp = QByteArray::fromRawData((char*)msg->msgHead, sizeof(msg->msgHead)).toHex();
            ui->tableWidget->setItem(2, 0, new QTableWidgetItem("msgHead"));
            ui->tableWidget->setItem(2, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));

            tmp = QByteArray::fromRawData((char*)msg->field0, sizeof(msg->field0)).toHex();
            ui->tableWidget->setItem(3, 0, new QTableWidgetItem("field0"));
            ui->tableWidget->setItem(3, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));

            #define F(x,y) \
            if(0 != msg->x##_l){ \
                tmp = QByteArray::fromRawData((char*)msg->x, msg->x##_l).toHex();\
                ui->tableWidget->setItem(y, 0, new QTableWidgetItem(#x));\
                ui->tableWidget->setItem(y, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));\
            }else{\
                ui->tableWidget->setItem(y, 0, new QTableWidgetItem(#x));\
                ui->tableWidget->setItem(y, 1, new QTableWidgetItem("empty"));\
            }
            F(field1, 4) F(field2, 5) F(field3, 6) F(field4, 7) F(field5, 8) F(field6, 9) F(field7, 10) F(field8, 11) F(field9, 12) F(field10, 13)
            F(field11, 14) F(field12, 15) F(field13, 16) F(field14, 17) F(field15, 18) F(field16, 19) F(field17, 20) F(field18, 21) F(field19, 22) F(field20, 23)
            F(field21, 24) F(field22, 25) F(field23, 26) F(field24, 27) F(field25, 28) F(field26, 29) F(field27, 30) F(field28, 31) F(field29, 32) F(field30, 33)
            F(field31, 34) F(field32, 35) F(field33, 36) F(field34, 37) F(field35, 38) F(field36, 39) F(field37, 40) F(field38, 41) F(field39, 42) F(field40, 43)
            F(field41, 44) F(field42, 45) F(field43, 46) F(field44, 47) F(field45, 48) F(field46, 49) F(field47, 50) F(field48, 51) F(field49, 52) F(field50, 53)
            F(field51, 54) F(field52, 55) F(field53, 56) F(field54, 57) F(field55, 58) F(field56, 59) F(field57, 60) F(field58, 61) F(field59, 62) F(field60, 63)
            F(field61, 64) F(field62, 65) F(field62, 66) F(field63, 67) F(field64, 68)
            #undef F
            break;
        }
        case CUSTOMER_CUP:
        default:{
            MsgJL* msg = static_cast<MsgJL*>(ptr);
            QByteArray tmp = QByteArray::fromRawData((char*)msg->msgLen, sizeof(msg->msgLen)).toHex();
            ui->tableWidget->setItem(0, 0, new QTableWidgetItem("msgLen"));
            ui->tableWidget->setItem(0, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));

            tmp = QByteArray::fromRawData((char*)msg->msgTPDU, sizeof(msg->msgTPDU)).toHex();
            ui->tableWidget->setItem(1, 0, new QTableWidgetItem("msgTPDU"));
            ui->tableWidget->setItem(1, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));

			tmp = QByteArray::fromRawData((char*)msg->msgHead, sizeof(msg->msgHead)).toHex();
            ui->tableWidget->setItem(2, 0, new QTableWidgetItem("msgHead"));
            ui->tableWidget->setItem(2, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));

            tmp = QByteArray::fromRawData((char*)msg->field0, sizeof(msg->field0)).toHex();
            ui->tableWidget->setItem(3, 0, new QTableWidgetItem("field0"));
            ui->tableWidget->setItem(3, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));

            #define F(x,y) \
            if(0 != msg->x##_l){ \
                tmp = QByteArray::fromRawData((char*)msg->x, msg->x##_l).toHex();\
                ui->tableWidget->setItem(y, 0, new QTableWidgetItem(#x));\
                ui->tableWidget->setItem(y, 1, new QTableWidgetItem(QString::fromUtf8(tmp).toUpper()));\
            }else{\
                ui->tableWidget->setItem(y, 0, new QTableWidgetItem(#x));\
                ui->tableWidget->setItem(y, 1, new QTableWidgetItem("empty"));\
            }
            F(field1, 4) F(field2, 5) F(field3, 6) F(field4, 7) F(field5, 8) F(field6, 9) F(field7, 10) F(field8, 11) F(field9, 12) F(field10, 13)
            F(field11, 14) F(field12, 15) F(field13, 16) F(field14, 17) F(field15, 18) F(field16, 19) F(field17, 20) F(field18, 21) F(field19, 22) F(field20, 23)
            F(field21, 24) F(field22, 25) F(field23, 26) F(field24, 27) F(field25, 28) F(field26, 29) F(field27, 30) F(field28, 31) F(field29, 32) F(field30, 33)
            F(field31, 34) F(field32, 35) F(field33, 36) F(field34, 37) F(field35, 38) F(field36, 39) F(field37, 40) F(field38, 41) F(field39, 42) F(field40, 43)
            F(field41, 44) F(field42, 45) F(field43, 46) F(field44, 47) F(field45, 48) F(field46, 49) F(field47, 50) F(field48, 51) F(field49, 52) F(field50, 53)
            F(field51, 54) F(field52, 55) F(field53, 56) F(field54, 57) F(field55, 58) F(field56, 59) F(field57, 60) F(field58, 61) F(field59, 62) F(field60, 63)
            F(field61, 64) F(field62, 65) F(field62, 66) F(field63, 67) F(field64, 68)
            #undef F
            break;
        }
    }
}
int DEC::MainWindow::decodeMsg(const QModelIndex& index)
{
    DEC::dataItem *currentItem = static_cast<DEC::dataItem*>(index.internalPointer());
    ui->textBrowser_2->setPlainText(NULL);
    ui->tableWidget->clearContents();
    if(currentItem != NULL){
        u32 msg8583Len = 0;
        const u8* msg8583 = ::TestTPDU(static_cast<CustomerID>(model->customerid), model->tpdu, currentItem->tcpData()->data, currentItem->tcpData()->dataLen, &msg8583Len);
        if(NULL != msg8583){
            if(OK == model->decoder(msg8583, msg8583Len, model->ptr2)){
                fillTable(model->ptr2);
            }else{
                ui->textBrowser_2->setPlainText("decoding(illegal fields) fail.");
            }
            ui->textBrowser_2->setPlainText(QString::asprintf("%s", ConsoleBuffer.buf));
            ClearConsoleBuf();
        }else{
            ui->textBrowser_2->setPlainText(QString::asprintf("decoding(TPDU %s test fail) fail.", model->tpdu));
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
    if(strlen(ui->textEdit->toPlainText().toLocal8Bit().data()) > MAXSINGLEMSGLEN){
        QMessageBox::information(this, "Title", "data too long.");
        return -1;
    }
    s8 tpdu[TMPMSGSIZE] = {0};
    strcpy(tpdu, ui->lineEdit_2->text().toLocal8Bit().data());
    s8 msgInput[MAXSINGLEMSGLEN] = {0};
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
    const u8* msg8583 = ::TestTPDU(static_cast<CustomerID>(model->customerid), tpdu, msg, len / 2, &msg8583Len);
    if(NULL != msg8583){
        InitConsoleBuf();
        if(OK == model->decoder(msg8583, msg8583Len, model->ptr2)){
            fillTable(model->ptr2);
        }
        else{
            ui->textBrowser_2->setPlainText("decoding(illegal fields) fail.");
        }
        ui->textBrowser_2->append(QString::asprintf("%s", ConsoleBuffer.buf));
        ClearConsoleBuf();
    }else{
        ui->textBrowser_2->append("decoding(TPDU test fail) fail.");
        return -1;
    }
    return 1;
}
int DEC::MainWindow::testTPDU()//test TPDU
{
    if(strlen(ui->textEdit->toPlainText().toLocal8Bit().data()) > MAXSINGLEMSGLEN){
        QMessageBox::information(this, "Title", "data too long.");
        return -1;
    }
    s8 tpdu[TMPMSGSIZE] = {0};
    strcpy(tpdu, ui->lineEdit_2->text().toLocal8Bit().data());
    s8 msgInput[MAXSINGLEMSGLEN] = {0};
    strcpy(msgInput, ui->textEdit->toPlainText().toLocal8Bit().data());
    u32 len = strlen(msgInput);
    if(len < 11){
        QMessageBox::information(this, "Title", "test TPDU(data too short) fail.");
        return -1;
    }
    u8* msg = (u8*)malloc(len / 2);
    if(!(1 == HexcharStringRevert(msgInput, msg, len / 2))){
        QMessageBox::information(this, "Title", "test TPDU(illegal hex string) fail.");
        return -1;
    }
    u32 msg8583Len = 0;
    const u8* msg8583 = ::TestTPDU(static_cast<CustomerID>(model->customerid), tpdu, msg, len / 2, &msg8583Len);
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
    if(strlen(ui->textEdit->toPlainText().toLocal8Bit().data()) > MAXSINGLEMSGLEN){
        QMessageBox::information(this, "Title", "data too long.");
        return -1;
    }
    s8 msgInput[MAXSINGLEMSGLEN] = {0};
    strcpy(msgInput, ui->textEdit->toPlainText().toLocal8Bit().data());
    s8 msgInputNew[MAXSINGLEMSGLEN] = {0};
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
    if(strlen(ui->textEdit->toPlainText().toLocal8Bit().data()) > MAXSINGLEMSGLEN){
        QMessageBox::information(this, "Title", "data too long.");
        return -1;
    }
    s8 msgInput[MAXSINGLEMSGLEN] = {0};
    strcpy(msgInput, ui->textEdit->toPlainText().toLocal8Bit().data());
    s8 msgInputNew[MAXSINGLEMSGLEN * 2] = {0};
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
void DEC::MainWindow::selectDecoder(int index){
    mutex.lock();
    switch(index){
        case -1: //JL
            strcpy(model->tpdu, "6000010000");
            model->customerid = CUSTOMER_JL;
            model->decoder = DecodeJLMsg;
            free(model->ptr1);
            free(model->ptr2);
            model->ptr1 = (void*)malloc(sizeof(MsgJL));
            model->ptr2 = (void*)malloc(sizeof(MsgJL));
            break;
        case 0: //JL
            strcpy(model->tpdu, "6000010000");
            model->customerid = CUSTOMER_JL;
            model->decoder = DecodeJLMsg;
            free(model->ptr1);
            free(model->ptr2);
            model->ptr1 = (void*)malloc(sizeof(MsgJL));
            model->ptr2 = (void*)malloc(sizeof(MsgJL));
            break;
        case 1: //CUP
            strcpy(model->tpdu, "6000020000");
            model->customerid = CUSTOMER_CUP;
            model->decoder = DecodeYLMsg;
            free(model->ptr1);
            free(model->ptr2);
            model->ptr1 = (void*)malloc(sizeof(MsgYL));
            model->ptr2 = (void*)malloc(sizeof(MsgYL));
            break;
        case 2: //YS
            strcpy(model->tpdu, "6000580000");
            model->customerid = CUSTOMER_YS;
            model->decoder = DecodeYSMsg;
            free(model->ptr1);
            free(model->ptr2);
            model->ptr1 = (void*)malloc(sizeof(MsgYS));
            model->ptr2 = (void*)malloc(sizeof(MsgYS));
            break;
        default: //JL
            strcpy(model->tpdu, "6000010000");
            model->customerid = CUSTOMER_JL;
            model->decoder = DecodeJLMsg;
            free(model->ptr1);
            free(model->ptr2);
            model->ptr1 = (void*)malloc(sizeof(MsgJL));
            model->ptr2 = (void*)malloc(sizeof(MsgJL));
            break;
    }
    mutex.unlock();
}
void DEC::MainWindow::showEvent(QShowEvent *event){
    myTimeId = startTimer(100);
}
void DEC::MainWindow::hideEvent(QHideEvent *event){
    killTimer(myTimeId);
    myTimeId = 0;
}
void DEC::MainWindow::timerEvent(QTimerEvent *event){
    if(event->timerId() == myTimeId  && model->rowCount() != treeViewUpdataFlag){
        treeViewUpdataFlag = model->rowCount();
        ui->treeView->verticalScrollBar()->setValue(ui->treeView->verticalScrollBar()->maximum());
    }else{
        QWidget::timerEvent(event);
    }
}