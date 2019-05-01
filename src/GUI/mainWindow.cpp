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
                b.portSrc));
            item->setText(4, "TCP");
            item->setText(5, b.extraInfo);
            item->setText(6, b.extraInfo);
            emit um->newData(item);
            cnt++;
            //Sleep(1);
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
    QRegExp regExp("tcp\\sport(\\s[0-9]{1,5})+");
    ui->lineEdit->setValidator(new QRegExpValidator(regExp, this)); //set regexp rule
    ui->textBrowser->setLineWrapMode(QTextEdit::FixedColumnWidth); //
    ui->textBrowser->setLineWrapColumnOrWidth(95); // warp each 32 bytes
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(findInterface()));
    connect(this, SIGNAL(interfaceScaned(bool)), this, SLOT(enabltPbt4(bool)));
    //QThread qth;
    //appendThread *aph = new appendThread(this, 1);
    //aph->moveToThread(&qth);
    connect(this, SIGNAL(newData(QTreeWidgetItem*)), this, SLOT(appendNewData(QTreeWidgetItem*)), Qt::DirectConnection);
    //qth.start();

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(loop()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(stop()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(restart()));
    connect(ui->treeWidget, SIGNAL(itemSelectionChanged()), this, SLOT(expandData()));
    connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(decodeMsg()));
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
    int in = ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->currentItem());
    if(in < vec.size()){
        u32 msgLen = 0;
        if(NULL == testTPDU("6010000001", vec[in].data, vec[in].dataLen, &msgLen)){
            QMessageBox::information(this, "Title", "TPDU 6010000000 test failed.");
        }else{
            QMessageBox::information(this, "Title", "TPDU 6010000000 test passed.");
        }
    }
    return 1;
}