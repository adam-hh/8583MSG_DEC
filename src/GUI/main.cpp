#include "QtWidgets/QApplication"
#include "mainWindow.h"
#include "interfaceDialog.h"
#include <pthread.h>

void *threadScan(void *arg){
    DEC::interfaceDialog::scanDev();
    //static_cast<DEC::MainWindow*>(arg)->disablePtt4();
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
    // if(pthread_join (scan, NULL)){
    //     printf("error joining thread.");
    //     abort();
    // }else{
    //     u->enablePbt4();
    // }    
    return app.exec();
}