#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "interfaceDialog.h"
#include <pthread.h>

DEC::MainWindow::MainWindow() :
    ui(new Ui::MainWindow),
    itf(new DEC::interfaceDialog(this))
{
    ui->setupUi(this);
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(findInterface()));
    connect(this, SIGNAL(interfaceScaned(bool)), this, SLOT(enabltPbt4(bool)));
    connect(this, SIGNAL(newData(QString)), this, SLOT(appendNewData(QString)));

    connect(ui->pushButton, SIGNAL(clicked()), this, SLOT(loop()));
    connect(ui->pushButton_2, SIGNAL(clicked()), this, SLOT(stop()));
    connect(ui->pushButton_3, SIGNAL(clicked()), this, SLOT(restart()));
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
void *threadLoop(void* arg)
{
    DEC::MainWindow *um = static_cast<DEC::MainWindow*>(arg);
    while(true)
    {
        //emit um->newData("Hello");
        Sleep(1000);
    }

}
int DEC::MainWindow::appendNewData(QTreeWidgetItem *item)
{
    //ui->textBrowser->append(str);
}
int DEC::MainWindow::loop()
{
    ui->pushButton->setEnabled(false);
    ui->pushButton_4->setEnabled(false);
    ui->pushButton_2->setEnabled(true);
    ui->pushButton_3->setEnabled(true);
    pthread_t pLoop;
    if (pthread_create(&pLoop, NULL, threadLoop, static_cast<void *>(this))) {
        printf("error creating thread.");
        abort();
    }



    return 1;
}
int DEC::MainWindow::expandData()
{
    QMessageBox::information(this, "Title", "Error opening adapter.");
    return 1;
}
int DEC::MainWindow::decode()
{
    return 1;
}
int DEC::MainWindow::stop()
{
    ui->pushButton_2->setEnabled(false);
    ui->pushButton_3->setEnabled(false);
    ui->pushButton->setEnabled(true);
    ui->pushButton_4->setEnabled(true);
    return 1;
}
int DEC::MainWindow::restart()
{
    ui->pushButton_4->setEnabled(false);
    ui->pushButton->setEnabled(false);
    ui->pushButton_2->setEnabled(true);
    return 1;
}