#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "interfaceDialog.h"

DEC::MainWindow::MainWindow() :
    ui(new Ui::MainWindow),
    itf(new DEC::interfaceDialog(this))
{
    ui->setupUi(this);
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(findInterface()));
    connect(this, SIGNAL(interfaceScaned()), this, SLOT(enabltPbt4()));
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
    itf->run();
}

void DEC::MainWindow::enabltPbt4()
{
    ui->pushButton_4->setEnabled(true);
}
void DEC::MainWindow::setPbt4(bool bl)
{
    if(bl)
        emit interfaceScaned();
}