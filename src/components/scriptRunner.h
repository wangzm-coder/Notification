#pragma once

#include <QDebug>
#include <QFile>
#include <QPointer>
#include <QProcess>
#include <QSharedPointer>
#include <QString>
#include <QThread>
#include <QTimer>

class ScriptRunner : public QThread {
    Q_OBJECT
public:
    ScriptRunner(QString scriptFile, int stopAfterSeconds = 0, int waitStopSeconds = 30, QObject *parent = nullptr);
    ~ScriptRunner();

protected:
    void run();

private:
    QString m_scriptFile;
    int m_stopAfterSeconds = 0;
    int m_waitStopSeconds = 30;
};
