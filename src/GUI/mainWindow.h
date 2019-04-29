#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets/QMainWindow>

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
        void setPbt4(bool bl);
        void setPbt(bool bl);
    public slots:
        void enabltPbt4(bool bl);
        int loop(); //loop
        int expandData(); //expand to treewidget
        int decode(); //decode show in textbrowser
        int stop(); //stop loop
        int restart(); // restart loop
    signals:
        void interfaceScaned(bool bl);
    private slots:
        void findInterface();

    private:
        Ui::MainWindow *ui;
        DEC::interfaceDialog *itf;
    };
}


#endif // MAINWINDOW_H