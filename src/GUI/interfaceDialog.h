#ifndef INTERFACEDIALOG_H
#define INTERFACEDIALOG_H
#include <QtWidgets/QDialog>

namespace Ui {
    class interfaceDialog;
}

namespace DEC {
class interfaceDialog : public QDialog{
    Q_OBJECT

    public:
        interfaceDialog(QWidget *parent = nullptr);
        ~interfaceDialog();
    private slots:
        void accept();
        void reject();
    private:
        Ui::interfaceDialog *ui;
    };
}
#endif