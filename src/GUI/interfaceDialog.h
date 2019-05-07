#ifndef INTERFACEDIALOG_H
#define INTERFACEDIALOG_H
#include <QtWidgets/QDialog>
#include <QtCore/QString>
#include <QtCore/QVector>
#include <QtCore/QThread>

#include "pcap.h"

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
        static pcap_if_t *alldevs;
        static int scanDev();
        void showDevList();
    private slots:
        void accept();
        void reject();
        void itemPress();
    private:
        Ui::interfaceDialog *ui;
    };
}
#endif