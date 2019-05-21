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
extern QVector<TcpDataBlock*> vec;
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
        int expandData(const QModelIndex&); //expand to treewidget
        int decode(); //never in use
        int stop(); //stop loop
        int restart(); //restart loop
        int decodeMsg(const QModelIndex&); //decode message from capture
        int decodeMsgManual(); //decode message from manual input
        int testTPDU();//test TPDU
        int trimTextEdit();//trim(remove space)
        int alignTextEdit();//align(add space)
        int clearTextEdit();//clear
    protected:
        void timerEvent(QTimerEvent *event);
        void showEvent(QShowEvent *event);
        void hideEvent(QHideEvent *event);
    private slots:
        void findInterface();
        void selectDecoder(int);

    private:
        DEC::interfaceDialog *itf;
        int myTimeId;
        volatile int treeViewUpdataFlag;
        QThread T1;
        void fillTable(void*);
    };
}
#endif // MAINWINDOW_H