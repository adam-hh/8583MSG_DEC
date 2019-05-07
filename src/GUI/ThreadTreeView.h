#include <QtCore/QObject>
#include <QtCore/QStringList>
#include <QtWidgets/QTreeView>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QDialog>
#include "mainWindow.h"
#include "dataItemModel.h"

extern QVector<tcpDataBlock*> vec; //store captured data
namespace DEC{
class ThreadTreeView : public QObject{
    Q_OBJECT
public:
    explicit ThreadTreeView(dataItemModel *dmodel) : model(dmodel){}
    ~ThreadTreeView(){}

public slots:
    void startCapture(){
        for(QVector<tcpDataBlock*>::iterator it = vec.begin(); it != vec.end(); it++){
            free((*it)->data);
        }
        qDeleteAll(vec);
        QVector<tcpDataBlock*>().swap(vec); //empty vec
        int rlt = 0;
        model->resetModel();
        while(true){
            tcpDataBlock *b = (tcpDataBlock*)malloc(sizeof(tcpDataBlock));
            rlt = readFromUserBuff(usbf, b);
            if(1 == rlt){
                vec.append(b);
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