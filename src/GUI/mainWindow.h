#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QScrollBar>
#include <QtCOre/QString>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QStringList>
#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtGui/QStandardItemModel>
#include <QtGui/QStandardItem>
#include <QtWidgets/QTreeView>
#include <QtCore/QMutex>
#include <QtCore/QMutexLocker>
#include "dataItem.h"
#include "dataItemModel.h"

#define MAXSINGLEMSGLEN 10240
namespace Ui {
    class MainWindow;
}
namespace DEC {
extern QMutex mutex;
class interfaceDialog;
class MainWindow : public QMainWindow{
    Q_OBJECT
    public:
        explicit MainWindow();
        ~MainWindow();
        Ui::MainWindow *ui;
        dataItemModel *model;
        void setPbt4(bool bl); //pushButton_4
        void setPbt(bool bl); //pushButton
    public slots:
        int loop(); //loop
        int appendNewData(DEC::dataItem*); // apend new data
        int appendNewDatas(QVector<DEC::dataItem*>);
        int expandData(); //expand to treewidget
        int decode(); //never in use
        int stop(); //stop loop
        int restart(); //restart loop
        int decodeMsg(); //decode message from capture
        int decodeMsgManual(); //decode message from manual
        int testTPDU();//test TPDU
        int trimTextEdit();//trim(remove space)
        int alignTextEdit();//align(add space)
        int clearTextEdit();//clear
    signals:
        void newData(DEC::dataItem*); //signal to notiry when new data captured
        void newDatas(QVector<DEC::dataItem*>);
    protected:
        void timerEvent(QTimerEvent *event);
        void showEvent(QShowEvent *event);
        void hideEvent(QHideEvent *event);
    private slots:
        void findInterface();

    private:        
        DEC::interfaceDialog *itf;
        int myTimeId;
        volatile int treeViewUpdataFlag;
        QThread T1;
    };
}
#endif // MAINWINDOW_H