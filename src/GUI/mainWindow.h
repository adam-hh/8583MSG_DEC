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
        void enablePbt4();
        void disablePtt4();
    private slots:
        void findInterface();

    private:
        Ui::MainWindow *ui;
        DEC::interfaceDialog *itf;
    };
}


#endif // MAINWINDOW_H