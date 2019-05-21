#include <QtCore/QStringList>
#include <QtCore/QDateTime>
#include "dataItem.h"
#include "8583dump.h"

DEC::dataItem::dataItem(TcpDataBlock *data, int rowNum, int color) : fdata(data), rowNum(rowNum), color(color){}
DEC::dataItem::~dataItem(){
    //free(fdata->data);
    //free(fdata);
}

