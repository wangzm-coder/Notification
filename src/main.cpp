#include "mainWindow/mainWindow.h"

#include <QApplication>
#include<QSharedMemory>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QApplication::setApplicationName("Notification");
    QSharedMemory sharedMemory(QApplication::applicationName());
    if (!sharedMemory.create(1)) {
        QMessageBox::warning(nullptr, "Warning", "The application is already running.");
        return 1; // 程序已运行，退出
    }

    qApp->setQuitOnLastWindowClosed(false);

    MainWindow w;
    w.show();

    return a.exec();
    
}
