#include <pthread.h>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMessageBox>
#include "mainWindow.h"
#include "interfaceDialog.h"
#include "8583dump.h"

void *threadScan(void *arg){
    DEC::interfaceDialog::scanDev();
    static_cast<DEC::MainWindow*>(arg)->setPbt4(true);
    return NULL;
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
		//printf("initUserBuff(10000) failed.\n");
    return app.exec();
}