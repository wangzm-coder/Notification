#include "mainWindow/mainWindow.h"

#include <QApplication>
#include <QDebug>
#include <QSharedMemory>

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QApplication::setApplicationName("Notification");
    QSharedMemory sharedMemory(QApplication::applicationName());
    int currentId = QApplication::applicationPid();
    if (!sharedMemory.create(1)) {
        auto res = QMessageBox::question(nullptr, "Question", "The application is already running, should kill the application?", QMessageBox::Yes, QMessageBox::No);
        if (res == QMessageBox::Yes) {
            QString pidCommand = QString("pgrep %1").arg(QApplication::applicationName());
            QProcess pidProcess;
            pidProcess.start("bash", QStringList() << "-c" << pidCommand);
            pidProcess.waitForFinished();
            QString pidOutput = pidProcess.readAllStandardOutput().trimmed();

            if (!pidOutput.isEmpty()) {
                if (!sharedMemory.attach()) {
                    qDebug() << "Failed to connect to shared memory:" << sharedMemory.errorString();
                }

                QStringList pids = pidOutput.split("\n", Qt::SkipEmptyParts);
                for (const QString &pid : pids) {
                    if (pid != QString::number(currentId)) {
                        QString killCommand = QString("kill -9 %1").arg(pid);
                        int killResult = QProcess::execute("bash", QStringList() << "-c" << killCommand);
                        if (killResult == 0) {
                            qDebug() << "Successfully killed process with PID:" << pid;
                        } else {
                            qDebug() << "Failed to kill process with PID:" << pid;
                        }
                    }
                }
            }
        } else {
            return 1; // 选择不杀死进程，退出程序
        }
    }

    qApp->setQuitOnLastWindowClosed(false);

    MainWindow w;
    // w.show();

    return a.exec();
}
