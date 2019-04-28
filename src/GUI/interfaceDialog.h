#ifndef INTERFACEDIALOG_H
#define INTERFACEDIALOG_H
#include <QtWidgets/QDialog>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QThread>

namespace Ui {
    class interfaceDialog;
}

namespace DEC {
class interfaceDialog : public QDialog{
    Q_OBJECT

    public:
        interfaceDialog(QWidget *parent = nullptr);
        ~interfaceDialog();
        static QVector<QVector<QString>> devList;
        static int scanDev();
        void run();
    private slots:
        void accept();
        void reject();
    private:
        Ui::interfaceDialog *ui;
    };
}
#endif