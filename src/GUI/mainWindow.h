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
        void setPbt4(bool bl); //pushButton_4
        void setPbt(bool bl); //pushButton
    public slots:
        void enabltPbt4(bool bl); // slot connect to interfaceScaned
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
        void interfaceScaned(bool bl); //signal to notify when opendev returned without error
        void newData(QTreeWidgetItem *item); //signal to notiry when new data captured
    private slots:
        void findInterface();

    private:
        Ui::MainWindow *ui;
        DEC::interfaceDialog *itf;
    };
}
#endif // MAINWINDOW_H