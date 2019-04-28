#include "interfaceDialog.h"
#include "ui_interfaceDialog.h"
#include <QtWidgets/QPushButton>
#include <QtWidgets/QHeaderView>
#include "pcap.h"

DEC::interfaceDialog::interfaceDialog(QWidget *parent) : 
    QDialog(parent),
    ui(new Ui::interfaceDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    ui->treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

    connect(ui->treeWidget, SIGNAL(itemPressed(QTreeWidgetItem*,int)), this, SLOT(itemPress()));
}

DEC::interfaceDialog::~interfaceDialog()
{
    delete ui;
}
static pcap_if_t *alldevs;
void DEC::interfaceDialog::accept()
{
    int in = ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->currentItem());
    pcap_if_t *p;
    int i = 0;
    for(p = alldevs, i = 0; p; p = p->next)
        i++;
    if((in<0) || (in>i))
        return;
    /* Jump to the selected adapter */
	for (p=alldevs, i=0; i< in ;p=p->next, i++);
    
}
void DEC::interfaceDialog::reject()
{
    this->setVisible(false);
}
void DEC::interfaceDialog::itemPress()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

QVector<QVector<QString>> DEC::interfaceDialog::devList = {};
int DEC::interfaceDialog::scanDev()
{
    pcap_if_t *alldevs;
    pcap_if_t *p;    
    //QString str[4]= {0};    
    char errbuf[PCAP_ERRBUF_SIZE + 1];
    if(pcap_findalldevs(&alldevs, errbuf) == -1)
    {
        fprintf(stderr, "Error in pcap_findalldevs: %s\n", errbuf);
        return -1;
    }
    if(alldevs == NULL)
    {
        fprintf(stderr, "Found 0 NIC device, check whether the Npcap driver was installed correctly.\n");
        return -1;
    }

    int i = 0;
    for(p = alldevs; p; p = p->next, ++i)
    {
        QVector<QString> str;
        /* No. */
        str.append(QString::asprintf("%d", i));
        /* Description */
        if (p->description)
            str.append(p->description);
        else
            str.append(p->description);
        /* Name */
        printf("%s\n",p->name);
        str.append(p->name);
        /* Comments */
        str.append("...");
        devList.append(str);
    }
    return 1;
}
void DEC::interfaceDialog::run()
{
    ui->treeWidget->clear();
    for(int it = 0; it < devList.count(); it++){
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, devList[it][0]);
        item->setText(1, devList[it][1]);
        item->setText(2, devList[it][2]);
        item->setText(3, devList[it][3]);
        ui->treeWidget->addTopLevelItem(item);
    }
}