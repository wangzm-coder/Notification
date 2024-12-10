#include "src/components/scriptRunner.h"

ScriptRunner::ScriptRunner(QString pythonFile, int stopAfterSeconds, int waitStopSeconds, QObject *parent) : m_scriptFile(pythonFile),
                                                                                                             m_stopAfterSeconds(stopAfterSeconds),
                                                                                                             m_waitStopSeconds(waitStopSeconds),
                                                                                                             QThread(parent) {
    start();
}

ScriptRunner::~ScriptRunner() {}

void ScriptRunner::run() {
    bool startSuccess = false;
    QPointer<QProcess> m_process = new QProcess();
    m_process->setProcessChannelMode(QProcess::MergedChannels);

    connect(m_process.data(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this, m_process](int exitCode, QProcess::ExitStatus exitStatus) {
        qDebug() << "\n-------------------";
        qDebug() << m_scriptFile;
        qDebug() << "Process finished with exit code:" << exitCode;
        qDebug() << "Exit status:" << exitStatus;
        qDebug() << "-------------------\n";
    });

    connect(m_process, &QProcess::readyReadStandardOutput, [m_process]() {
        QString output = QString::fromUtf8(m_process->readAllStandardOutput());
        QStringList lines = output.split("\n", QString::SkipEmptyParts);
        for (const QString &line : lines) {
            qDebug() << line;
        }
    });

    connect(m_process, &QProcess::readyReadStandardError, [m_process]() {
        QString output = QString::fromUtf8(m_process->readAllStandardError());
        QStringList lines = output.split("\n", QString::SkipEmptyParts);
        for (const QString &line : lines) {
            qDebug() << line;
        }
    });

    if (QFile::exists(m_scriptFile)) {
        if (m_scriptFile.endsWith(".py", Qt::CaseInsensitive)) {
            qDebug() << "start python script...";
            m_process->start("python3", QStringList() << m_scriptFile);
        } else if (m_scriptFile.endsWith(".sh", Qt::CaseInsensitive)) {
            qDebug() << "start bash script...";
            m_process->start("bash", QStringList() << m_scriptFile);
        } else if (m_scriptFile.endsWith(".desktop", Qt::CaseInsensitive)) {
            qDebug() << "start desktop file...";
            m_process->start("xdg-open", QStringList() << m_scriptFile);
        } else {
            qDebug() << "Unsupported script type: " << m_scriptFile;
        }
        if (m_process->waitForStarted()) {
            startSuccess = true;
        }
    } else {
        qDebug() << "不存在脚本文件:" << m_scriptFile;
    }
    if (startSuccess) {
        if (m_stopAfterSeconds > 0) {
            QTimer::singleShot(m_stopAfterSeconds * 1000, [this]() {
                qDebug() << "QProcesss: timeout ,kill process!";
                quit();
            });
        }
        exec();
    }
    m_process->terminate();
    if (!m_process->waitForFinished(m_waitStopSeconds * 1000)) {
        m_process->kill();
    }
    m_process->deleteLater();
}