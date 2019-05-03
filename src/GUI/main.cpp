#include <pthread.h>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtCore/QMetaObject>
#include "mainWindow.h"
#include "ui_mainwindow.h"
#include "interfaceDialog.h"
#include "8583dump.h"
#include "pthread.h"

void *threadScan(void *arg){
    DEC::interfaceDialog::scanDev();
    DEC::MainWindow *um = static_cast<DEC::MainWindow*>(arg);
    QMetaObject::invokeMethod(um->ui->pushButton_4, "setEnabled", Qt::QueuedConnection, Q_ARG(bool, true));
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    DEC::MainWindow *u = new DEC::MainWindow;
    u->show();
    pthread_t scan;
    if (pthread_create(&scan, NULL, threadScan, static_cast<void *>(u))) {
        printf("error creating thread.");
        abort();
    }
    if((usbf = initUserBuff(100000)) == NULL)
        QMessageBox::information(u, "Title", "initUserBuff(100000) failed.");
    return app.exec();
}
