#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QDialog>
#include "mainWindow.h"
#include "dataItemModel.h"

namespace DEC{
class ThreadTreeView : public QObject{
    Q_OBJECT
public:
    explicit ThreadTreeView(dataItemModel *dmodel) : model(dmodel){}
    ~ThreadTreeView(){}

public slots:
    void startCapture(){
        for(QVector<TcpDataBlock*>::iterator it = vec.begin(); it != vec.end(); it++){
            free((*it)->data);
        }
        qDeleteAll(vec);
        QVector<TcpDataBlock*>().swap(vec); //empty vec
        int rlt = 0;
        model->resetModel();
        while(true){
            TcpDataBlock *b = (TcpDataBlock*)malloc(sizeof(TcpDataBlock));
            rlt = ReadFromUserBuff(Usbf, b);
            if(1 == rlt){
                DEC::vec.append(b);
                if(model->appendItem(b) != model->rowCount())
                    Sleep(100);
                //Sleep(10);
            }else{
                Sleep(100);
                pthread_testcancel();
            }
        }
    }

private:
    dataItemModel *model;
};
}