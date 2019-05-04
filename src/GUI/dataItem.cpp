#include <QtCore/QStringList>
#include <QtCore/QDateTime>
#include "dataItem.h"
#include "8583dump.h"

DEC::dataItem::dataItem(DEC::dataItem *parent){
    mParentItem = parent;
    mPtr = NULL;
    mLevel = 0;
    mRow = 0;
}
DEC::dataItem::~dataItem(){
    if(!mChildItems.isEmpty()){
        qDeleteAll(mChildItems.begin(), mChildItems.end());
        //mChildItems.clear();
        //QList<DEC::dataItem*>().swap(mChildItems);
    }else
        return;
}

void DEC::dataItem::appendChild(DEC::dataItem *item){
    item->setRow(mChildItems.size());
    mChildItems.append(item);
}
void DEC::dataItem::appendChilds(QVector<DEC::dataItem*> childs){
    for(QVector<DEC::dataItem*>::iterator it = childs.begin(); it != childs.end(); it++){
        (*it)->setRow(mChildItems.size());
        mChildItems.append(*it);
    }
}
void DEC::dataItem::removeChilds(){
    qDeleteAll(mChildItems.begin(), mChildItems.end());
    mChildItems.clear();
    QList<DEC::dataItem*>().swap(mChildItems);
}
DEC::dataItem *DEC::dataItem::child(int row){
    return mChildItems.value(row);
}
DEC::dataItem *DEC::dataItem::parentItem(){
    return mParentItem;
}
int DEC::dataItem::childCount() const{
    return mChildItems.count();
}
int DEC::dataItem::row() const{
    return mRow;
}
QVariant DEC::dataItem::data(int column) const{
    if(mLevel == 1){
        tcpDataBlock *tcpData = (tcpDataBlock*)mPtr;
        switch(column){
            case 0: return QString::number(mRow);
            case 1: return QString::fromLocal8Bit(tcpData->extraInfo);
            case 2: return QString::asprintf("%d.%d.%d.%d:%d",
                tcpData->ipSrc.byte1,
                tcpData->ipSrc.byte2,
                tcpData->ipSrc.byte3,
                tcpData->ipSrc.byte4,
                tcpData->portSrc);
            case 3: return QString::asprintf("%d.%d.%d.%d:%d",
                tcpData->ipDst.byte1,
                tcpData->ipDst.byte2,
                tcpData->ipDst.byte3,
                tcpData->ipDst.byte4,
                tcpData->portDst);
            case 4: return "TCP";
            case 5: return QString::fromLocal8Bit(tcpData->extraInfo);
            case 6: return QString::fromLocal8Bit(tcpData->extraInfo);
            default: return QVariant();
        }
    }
    return QVariant();
}