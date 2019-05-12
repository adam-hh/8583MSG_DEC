#include "dataItem.h"
#include "dataItemModel.h"
#include <QtGui/QColor>
#include <QtCore/QDateTime>
#include <QtCore/QStringList>

DEC::dataItemModel::dataItemModel(QStringList headers, QObject *parent)
    : QAbstractItemModel(parent), mHeaders(headers){
        columnNumCnt = 7;
}
DEC::dataItemModel::~dataItemModel(){
    resetModel();
}
DEC::dataItem *DEC::dataItemModel::itemFromIndex(const QModelIndex &index) const{
    if(!index.isValid())
        return NULL;
    dataItem *item = static_cast<dataItem*>(index.internalPointer());
    return item;
}
QVariant DEC::dataItemModel::headerData(int section, Qt::Orientation orientation, int role) const{
    if(orientation == Qt::Horizontal){
        if(role == Qt::DisplayRole){
            return mHeaders.at(section);
        }
    }
    return QVariant();
}
QVariant DEC::dataItemModel::data(const QModelIndex &index, int role) const{
    if(!index.isValid())
        return QVariant();
    dataItem *item = static_cast<dataItem*>(index.internalPointer());
    if(role == Qt::DisplayRole){
        switch(index.column()){
            case 0: return QString::number(item->getRow());
            case 1: return QString::fromLocal8Bit(item->tcpData()->timestr);
            case 2: return QString::asprintf("%d.%d.%d.%d:%d",
                                item->tcpData()->ipSrc.byte1,
                                item->tcpData()->ipSrc.byte2,
                                item->tcpData()->ipSrc.byte3,
                                item->tcpData()->ipSrc.byte4,
                                item->tcpData()->portSrc
                            );
            case 3: return QString::asprintf("%d.%d.%d.%d:%d",
                                item->tcpData()->ipDst.byte1,
                                item->tcpData()->ipDst.byte2,
                                item->tcpData()->ipDst.byte3,
                                item->tcpData()->ipDst.byte4,
                                item->tcpData()->portDst
                            );
            case 4: return "TCP";
            case 5: return QString::number(item->tcpData()->totallen);
            case 6: return QString::number(item->tcpData()->tv_usec);
            default:
                    return QVariant();
        }
    }
    if(role == Qt::BackgroundRole){
        switch(item->getColor()){
            case GREEN: return QBrush(QColor("#00FF7F"));//SpringGreen1
            case YELLOW: return QBrush(QColor("#FFFF00"));//Yellow
            default: return QBrush(QColor("#F5F5F5"));//WhiteSmoke
        }
    }
    return QVariant();
}

Qt::ItemFlags DEC::dataItemModel::flags(const QModelIndex &index) const{
    if(!index.isValid())
        return 0;
    return QAbstractItemModel::flags(index);
}
QModelIndex DEC::dataItemModel::index(int row, int column, const QModelIndex &parent) const{
    if(row >= allRows.count() || row < 0 || column >= columnNumCnt || column < 0)
        return QModelIndex();
    DEC::dataItem *record;
    record = allRows[row];
    return createIndex(row, column, record);
}
QModelIndex DEC::dataItemModel::parent(const QModelIndex &index) const{
    return QModelIndex();
}
int DEC::dataItemModel::rowCount(const QModelIndex &parent) const {
    if(parent.column() >= columnNumCnt)
        return 0;
    return allRows.count();
}
int DEC::dataItemModel::columnCount(const QModelIndex &parent) const{
    return columnNumCnt;
}

void DEC::dataItemModel::resetModel(){
    //beginResetModel();
    qDeleteAll(allRows);
    allRows.resize(0);
    qDeleteAll(newRows);
    newRows.resize(0);
    endResetModel();
}
int DEC::dataItemModel::appendItem(tcpDataBlock* item){
    DEC::dataItem *record = new DEC::dataItem(item);
    int pos = -1;
    mutex.lock();
    u32 msg8583Len = 0;
    u8* msg8583 = ::testTPDU(tpdu, item->data, item->dataLen, &msg8583Len);
    if(NULL != msg8583){
        MsgJL msgjl = {0};
        initConsoleBuf();
        if(OK == decode(msg8583, msg8583Len, &msgjl)){
            record->setColor(GREEN);
        }else{
            record->setColor(YELLOW);
        }
    }
    newRows << record;
    if(newRows.count() < 2){
        QTimer::singleShot(0, this, SLOT(flushVisibleRows()));
    }
    mutex.unlock();
    pos = allRows.count() + newRows.count() - 1;
    return pos;
}
void DEC::dataItemModel::flushVisibleRows(){
    mutex.lock();
    int pos = allRows.count();
    if(newRows.count() > 0){
        beginInsertRows(QModelIndex(), pos, pos + newRows.count());
        foreach(DEC::dataItem *record, newRows){
            record->setRow(pos++);
            allRows << record;
        }
        endInsertRows();
        newRows.resize(0);
        mutex.unlock();
    }
}
QVector<DEC::dataItem*> DEC::dataItemModel::allRows;
QVector<DEC::dataItem*> DEC::dataItemModel::newRows;
s8 DEC::dataItemModel::tpdu[11] = "6000010000";
int (*DEC::dataItemModel::decode)(const u8*, u32, MsgJL*)  = ::DecodeJLMsg;