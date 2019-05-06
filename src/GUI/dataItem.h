#ifndef DATAITEM_H
#define DATAITEM_H
#include <QtCore/QVariant>
#include <QtCore/QVector>
#include <QtCore/QList>
#include "8583.h"
#include "8583dump.h"
namespace DEC
{
class dataItem{
public:
    explicit dataItem(tcpDataBlock* data = nullptr);
    ~dataItem();

    tcpDataBlock* tcpData(){return fdata;}
    void setRow(int row){
        rowNum = row;
    }
    int getRow(){
        return rowNum;
    }
private:
    tcpDataBlock* fdata;
    int rowNum;
};

} // namespace DEC

#endif