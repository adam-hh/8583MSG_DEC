#ifndef DATAITEM_H
#define DATAITEM_H
#include <QtCore/QVariant>
#include <QtCore/QVector>
#include <QtCore/QList>
#include "8583.h"
#include "8583dump.h"

namespace DEC
{
typedef enum{
    GRAY = 0,
    WHITE,
    GREEN,
    YELLOW
}DecColor;
class dataItem{
public:
    explicit dataItem(TcpDataBlock* data = nullptr, int rowNum = 0, int color = GRAY);
    ~dataItem();

    TcpDataBlock* tcpData(){return fdata;}
    void setRow(int row){
        rowNum = row;
    }
    int getRow(){
        return rowNum;
    }
    void setColor(DecColor clr){
        color = clr;
    }
    int getColor(){
        return color;
    }
private:
    TcpDataBlock* fdata;//raw data source
    int rowNum;//row number
    int color;//background color
};

} // namespace DEC

#endif