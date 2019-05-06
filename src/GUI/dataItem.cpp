#include <QtCore/QStringList>
#include <QtCore/QDateTime>
#include "dataItem.h"
#include "8583dump.h"

DEC::dataItem::dataItem(tcpDataBlock *data) : fdata(data){}
DEC::dataItem::~dataItem(){
    //free(fdata->data);
    //free(fdata);
}

