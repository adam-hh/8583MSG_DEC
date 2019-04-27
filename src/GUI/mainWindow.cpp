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
}