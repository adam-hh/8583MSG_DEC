#include "dataItem.h"
#include "dataItemModel.h"
#include <QtGui/QColor>
#include <QtCore/QDateTime>
#include <QtCore/QStringList>

DEC::dataItemModel::dataItemModel(QStringList headers, QObject *parent)
    : QAbstractItemModel(parent){
        mHeaders = headers;
        mRootItem = new dataItem;
}
DEC::dataItemModel::~dataItemModel(){
    delete mRootItem;
}
DEC::dataItem *DEC::dataItemModel::itemFromIndex(const QModelIndex &index) const{
    if(!index.isValid())
        return NULL;
    dataItem *item = static_cast<dataItem*>(index.internalPointer());
    return item;
}
DEC::dataItem *DEC::dataItemModel::root(){
    return mRootItem;
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
        return item->data(index.column());
    }
    return QVariant();
}

Qt::ItemFlags DEC::dataItemModel::flags(const QModelIndex &index) const{
    if(!index.isValid())
        return 0;
    return QAbstractItemModel::flags(index);
}
QModelIndex DEC::dataItemModel::index(int row, int column, const QModelIndex &parent) const{
    if(!hasIndex(row, column, parent))
        return QModelIndex();
    dataItem *parentItem;
    if(!parent.isValid())
        parentItem = mRootItem;
    else
        parentItem = static_cast<dataItem*>(parent.internalPointer());
    
    dataItem *childItem = parentItem->child(row);
    if(childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}
QModelIndex DEC::dataItemModel::parent(const QModelIndex &index) const{
    if(!index.isValid())
        return QModelIndex();
    dataItem *childItem = static_cast<dataItem*>(index.internalPointer());
    dataItem *parentItem = childItem->parentItem();

    if(parentItem == mRootItem)
        return QModelIndex();
    return createIndex(parentItem->row(), 0, parentItem);
}
int DEC::dataItemModel::rowCount(const QModelIndex &parent) const {
    dataItem *parentItem;
    if(parent.column() > 0)
        return 0;
    
    if(!parent.isValid())
        parentItem = mRootItem;
    else
        parentItem = static_cast<dataItem*>(parent.internalPointer());
    return parentItem->childCount();
}
int DEC::dataItemModel::columnCount(const QModelIndex &parent) const{
    return mHeaders.size();
}
void DEC::dataItemModel::beginInsertRowss(const QModelIndex &parent, int first, int last){
    beginInsertRows(parent, first, last);
}
void DEC::dataItemModel::endInsertRowss(){
    endInsertRows();
}
void DEC::dataItemModel::resetModel(){
    mRootItem = new dataItem;
    endResetModel();
}