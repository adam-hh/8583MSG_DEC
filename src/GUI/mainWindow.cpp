#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "interfaceDialog.h"

DEC::MainWindow::MainWindow() :
    ui(new Ui::MainWindow),
    itf(new DEC::interfaceDialog(this))
{
    ui->setupUi(this);
    connect(ui->pushButton_4, SIGNAL(clicked()), this, SLOT(findInterface()));
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
void DEC::MainWindow::enablePbt4()
{
    ui->pushButton_4->setEnabled(true);
}
void DEC::MainWindow::disablePtt4()
{
    ui->pushButton_4->setEnabled(false);
}