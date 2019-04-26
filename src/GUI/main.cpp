#include "QtWidgets/QApplication"
#include "mainWindow.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    MainWindow *u = new MainWindow;
    u->show();
    return app.exec();
}