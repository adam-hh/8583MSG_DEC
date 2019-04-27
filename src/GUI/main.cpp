#include "QtWidgets/QApplication"
#include "mainWindow.h"
#include "interfaceDialog.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    DEC::MainWindow *u = new DEC::MainWindow;
    u->show();
    return app.exec();
}