#ifndef DATAITEM_H
#define DATAITEM_H
#include <QtCore/QVariant>
#include <QtCore/QVector>
#include <QtCore/QList>
#include "8583.h"
#include "8583dump.h"
typedef enum{
Gray = 0,
White,
Green,
Red
}DECCOLOR;
namespace DEC
{
class dataItem{
public:
    explicit dataItem(tcpDataBlock* data = nullptr, int rowNum = 0, int color = 0);
    ~dataItem();

    tcpDataBlock* tcpData(){return fdata;}
    void setRow(int row){
        rowNum = row;
    }
    int getRow(){
        return rowNum;
    }
    void setColor(DECCOLOR clr){
        color = clr;
    }
    int getColor(){
        return color;
    }
private:
    tcpDataBlock* fdata;
    int rowNum;
    int color;
};

} // namespace DEC

#endif