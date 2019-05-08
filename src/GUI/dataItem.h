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
}DECCOLOR;
class dataItem{
public:
    explicit dataItem(tcpDataBlock* data = nullptr, int rowNum = 0, int color = GRAY);
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
    tcpDataBlock* fdata;//raw data source
    int rowNum;//row number
    int color;//background color
};

} // namespace DEC

#endif