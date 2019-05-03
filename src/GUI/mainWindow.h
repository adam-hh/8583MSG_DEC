#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QScrollBar>
#include <QtCOre/QString>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtGui/QBrush>
#include <QtGui/QColor>
#include <QtGui/QStandardItemModel>
#include <QtWidgets/QTreeView>
#define MAXSINGLEMSGLEN 10240
namespace Ui {
    class MainWindow;
}
namespace DEC {
class interfaceDialog;
class MainWindow : public QMainWindow{
    Q_OBJECT
    public:
        explicit MainWindow();
        ~MainWindow();
        Ui::MainWindow *ui;
        void setPbt4(bool bl); //pushButton_4
        void setPbt(bool bl); //pushButton
    public slots:
        int loop(); //loop
        int appendNewData(QTreeWidgetItem *item); // apend new data
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
        void newData(QTreeWidgetItem *item); //signal to notiry when new data captured
    private slots:
        void findInterface();

    private:        
        DEC::interfaceDialog *itf;
    };
}
#endif // MAINWINDOW_H