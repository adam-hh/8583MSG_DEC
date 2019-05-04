#ifndef DATAITEM_H
#define DATAITEM_H
#include <QtCore/QVariant>
#include <QtCore/QVector>
#include <QtCore/QList>
namespace DEC
{
class dataItem{
public:
    explicit dataItem(dataItem *parentItem = nullptr);
    ~dataItem();
    void appendChild(dataItem *child);
    void appendChilds(QVector<DEC::dataItem*> childs);
    void removeChilds();

    dataItem *child(int row);
    dataItem *parentItem();
    int childCount() const;
    int row() const;

    QVariant data(int column) const;

    int level(){return mLevel;}
    void setLevel(int level){mLevel = level;}

    void setPtr(void* p) { mPtr = p;}
    void* ptr(){return mPtr;}

    void setRow(int row){
        mRow = row;
    }
    QList<dataItem*> mChildItems;
private:
    dataItem *mParentItem;
    int mLevel;
    void* mPtr;
    int mRow;
};

} // namespace DEC

#endif