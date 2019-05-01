#include "appendThread.h"
appendThread::appendThread(QObject *parent) : QObject(parent){

}
appendThread::appendThread(DEC::MainWindow *parent, int i) : m(parent){

}
appendThread::~appendThread(){

}
appendThread::appendNew(QTreeWidgetItem *item){
    //DEC::MainWindow *m;
    //m = static_cast<DEC::MainWindow*>(parent);
    //m = (DEC::MainWindow*)(parent);
    m->appendNewData(item);
}