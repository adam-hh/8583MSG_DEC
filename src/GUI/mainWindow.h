#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QTreeWidgetItem>
#include <QtWidgets/QScrollBar>
#include <QtCOre/QString>

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
        int decode(); //decode show in textbrowser
        int stop(); //stop loop
        int restart(); //restart loop
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