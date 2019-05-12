#include <pthread.h>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include <QtCore/QMetaObject>
#include <QtCore/QGenericArgument>
#include "mainWindow.h"
#include "ui_mainwindow.h"
#include "interfaceDialog.h"
#include "8583dump.h"
#include <pthread.h>

void *threadScan(void *arg){
    DEC::interfaceDialog::scanDev();
    DEC::MainWindow *um = static_cast<DEC::MainWindow*>(arg);
    QMetaObject::invokeMethod(um->ui->pushButton_4, "setEnabled", Qt::QueuedConnection, Q_ARG(bool, true));
    return NULL;
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    DEC::MainWindow *u = new DEC::MainWindow;
    u->show();
    pthread_t scan;
    pthread_detach(scan);
    if (pthread_create(&scan, NULL, threadScan, static_cast<void *>(u))) {
        printf("error creating thread.");
        abort();
    }
    return app.exec();
}
