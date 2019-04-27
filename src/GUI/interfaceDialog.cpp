#include "interfaceDialog.h"
#include "ui_interfaceDialog.h"
#include <QtWidgets/QPushButton>

DEC::interfaceDialog::interfaceDialog(QWidget *parent) : 
    QDialog(parent),
    ui(new Ui::interfaceDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
}

DEC::interfaceDialog::~interfaceDialog()
{
    delete ui;
}

void DEC::interfaceDialog::accept()
{
    ;
}
void DEC::interfaceDialog::reject()
{
    this->setVisible(false);
}