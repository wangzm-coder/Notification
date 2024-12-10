#include "src/components/scriptRunner.h"

ScriptRunner::ScriptRunner(QString pythonFile, int stopAfterSeconds, int waitStopSeconds, QObject *parent) : m_scriptFile(pythonFile),
                                                                                                             m_stopAfterSeconds(stopAfterSeconds),
                                                                                                             m_waitStopSeconds(waitStopSeconds),
                                                                                                             QThread(parent) {
    start();
}

ScriptRunner::~ScriptRunner() {
    emit showInfo(QString("script file:%1 killed!").arg(m_scriptFile));
}

void ScriptRunner::run() {
    bool startSuccess = false;
    QPointer<QProcess> m_process = new QProcess();
    m_process->setProcessChannelMode(QProcess::MergedChannels);

    connect(m_process.data(), QOverload<int, QProcess::ExitStatus>::of(&QProcess::finished), [this, m_process](int exitCode, QProcess::ExitStatus exitStatus) {
        emit showInfo(QString("script file:%1, exit code:%2, Exit status:%3").arg(m_scriptFile).arg(exitCode).arg(exitStatus));
    });

    connect(m_process, &QProcess::readyReadStandardOutput, [this, m_process]() {
        QString output = QString::fromUtf8(m_process->readAllStandardOutput());
        QStringList lines = output.split("\n", QString::SkipEmptyParts);
        for (const QString &line : lines) {
            emit showInfo(line);
        }
    });

    connect(m_process, &QProcess::readyReadStandardError, [this, m_process]() {
        QString output = QString::fromUtf8(m_process->readAllStandardError());
        QStringList lines = output.split("\n", QString::SkipEmptyParts);
        for (const QString &line : lines) {
            emit showInfo(line);
        }
    });

    if (QFile::exists(m_scriptFile)) {
        if (m_scriptFile.endsWith(".py", Qt::CaseInsensitive)) {
            emit showInfo("start python script...");
            m_process->start("python3", QStringList() << m_scriptFile);
        } else if (m_scriptFile.endsWith(".sh", Qt::CaseInsensitive)) {
            emit showInfo("start bash script...");
            m_process->start("bash", QStringList() << m_scriptFile);
        } else if (m_scriptFile.endsWith(".desktop", Qt::CaseInsensitive)) {
            emit showInfo("start desktop file...");
            m_process->start("xdg-open", QStringList() << m_scriptFile);
        } else {
            emit showInfo(QString("Unsupported script type: %1").arg(m_scriptFile));
        }
        if (m_process->waitForStarted()) {
            startSuccess = true;
        }
    } else {
        emit showInfo(QString("不存在脚本文件:%1").arg(m_scriptFile));
    }
    if (startSuccess) {
        if (m_stopAfterSeconds > 0) {
            QTimer::singleShot(m_stopAfterSeconds * 1000, [this]() {
                emit showInfo("QProcesss: timeout ,kill process!");
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