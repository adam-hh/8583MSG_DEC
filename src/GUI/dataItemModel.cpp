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
    //qDeleteAll(newRows);
    //qDeleteAll(allRows);
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
            case 1: return QString::fromLocal8Bit(item->tcpData()->extraInfo);
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
            case 5: return QString::fromLocal8Bit(item->tcpData()->extraInfo);
            case 6: return QString::fromLocal8Bit(item->tcpData()->extraInfo);
            default:
                    return QVariant();
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
    DEC::dataItem *record = allRows[row];
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
    qDeleteAll(allRows);
    allRows.clear();
    allRows.squeeze();
    qDeleteAll(newRows);
    newRows.clear();
    newRows.squeeze();
    endResetModel();
}
int DEC::dataItemModel::appendItem(tcpDataBlock* item){
    DEC::dataItem *record = new DEC::dataItem(item);
    int pos = -1;
    newRows << record;
    if(newRows.count() < 2){
        QTimer::singleShot(0, this, SLOT(flushVisibleRows()));
    }
    pos = allRows.count() + newRows.count() - 1;
    return pos;
}
void DEC::dataItemModel::flushVisibleRows(){
    int pos = allRows.count();
    if(newRows.count() > 0 && newRows.count() < 100){
        beginInsertRows(QModelIndex(), pos, pos + newRows.count());
        foreach(DEC::dataItem *record, newRows){
            record->setRow(pos++);
            allRows << record;
        }
        endInsertRows();
        newRows.resize(0);
    }
}