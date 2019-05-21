#include <QtWidgets/QPushButton>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QMessageBox>
#include "interfaceDialog.h"
#include "ui_interfaceDialog.h"
#include "mainWindow.h"
#include "8583dump.h"
pcap_if_t *DEC::interfaceDialog::alldevs = NULL;
QVector<QVector<QString>> DEC::interfaceDialog::devList = {};
DEC::interfaceDialog::interfaceDialog(QWidget *parent) : 
    QDialog(parent),
    ui(new Ui::interfaceDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setText("确定");
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    ui->buttonBox->button(QDialogButtonBox::Cancel)->setText("取消");
    ui->treeWidget->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
    connect(ui->treeWidget, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(accept()));
    connect(ui->treeWidget, SIGNAL(itemPressed(QTreeWidgetItem*,int)), this, SLOT(itemPress()));
}

DEC::interfaceDialog::~interfaceDialog()
{
    delete ui;
    pcap_freealldevs(alldevs);
}
void DEC::interfaceDialog::accept()
{
    DEC::MainWindow *thisParent = static_cast<DEC::MainWindow*>(parentWidget());
    int in = ui->treeWidget->indexOfTopLevelItem(ui->treeWidget->currentItem());
    pcap_if_t *p;
    char errbuf[PCAP_ERRBUF_SIZE + 1];
    int i = 0;
    for(p = alldevs, i = 0; p; p = p->next)
        i++;
    if((in<0) || (in>i)){
        QMessageBox::information(this, "Title", QString::asprintf("Illegal Index %d of %d", in, i));
        return;
    }
    // Jump to the selected adapter
    for (p=alldevs, i=0; i<in ;p=p->next, i++);
    // Open the selected adapter
    if ((PHandle.handle = pcap_open_live(p->name,	// name of the device
        65536,                          // portion of the packet to capture.
                                        // 65536 grants that the whole packet will be captured on all the MACs.
        1,                              // promiscuous mode (nonzero means promiscuous)
        1000,                           // read timeout
        errbuf                          // error buffer
        )) == NULL){
        fprintf(stderr,"\nError opening adapter\n");
        QMessageBox::information(this, "Title", "Error opening adapter.");
        thisParent->setPbt(false);
        PHandle.avaliable = -1;
        return;
    }
    PHandle.avaliable = 1;
    this->setVisible(false);
    thisParent->setPbt(true);
}
void DEC::interfaceDialog::reject()
{
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
    this->setVisible(false);
}
void DEC::interfaceDialog::itemPress()
{
    if(ui->buttonBox->button(QDialogButtonBox::Ok)->isEnabled() == false)
        ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}
int DEC::interfaceDialog::scanDev()
{
    pcap_if_t *p;
    #ifdef WIN32
    // Load Npcap and its functions.
    if (!LoadNpcapDll())
    {
        fprintf(stderr, "Couldn't load Npcap\n");
        return -1;
    }
    #endif
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
            str.append("unkonwn");
        /* Name */
        str.append(p->name);
        /* Comments */
        str.append("...");
        devList.append(str);
    }
    return 1;
}
void DEC::interfaceDialog::showDevList()
{
    ui->treeWidget->clear();
    foreach(QVector<QString> vec, devList){
        QTreeWidgetItem *item = new QTreeWidgetItem();
        item->setText(0, vec[0]);
        item->setText(1, vec[1]);
        item->setText(2, vec[2]);
        item->setText(3, vec[3]);
        ui->treeWidget->addTopLevelItem(item);
    }
}