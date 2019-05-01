#include <QtWidgets/QMainWindow>
#include <QtCore/QObject>
#include "mainWindow.h"

class appendThread : public QObject
{
    Q_OBJECT
public:
    appendThread(QObject *parent = nullptr);
    appendThread(DEC::MainWindow *parent, int i);
    ~appendThread();
public  slots:
    int appendNew(QTreeWidgetItem *item);
private:
    DEC::MainWindow *m;
};