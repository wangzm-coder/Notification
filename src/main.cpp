#include "mainWindow/MainWindow.h"

#include <QApplication>
#include <QSystemTrayIcon>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    qApp->setQuitOnLastWindowClosed(false);
    MainWindow w;
    w.show();
    return a.exec();
}
