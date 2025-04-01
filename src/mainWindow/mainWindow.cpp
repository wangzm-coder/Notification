#include "mainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_timer(new QTimer(this)),
      m_infoTextWidget(new InfoTextWidget(this)) {
    ui->setupUi(this);
    setWindowIcon(QIcon(":/icons/info-white.png"));
    setWindowTitle(QApplication::applicationName());

    _initSystemTrayIcon();
    _initTableWidget();

    ui->splitter->setSizes({1, 0});
    ui->plainTextEdit->setReadOnly(true);

    connect(ui->action_add, &QAction::triggered, this, &MainWindow::on_addOneRow);
    connect(ui->action_save, &QAction::triggered, this, &MainWindow::on_save);
    connect(ui->action_delete, &QAction::triggered, this, &MainWindow::on_delete);

    _loadDataJsonFile();

    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::on_checkTime);
    m_timer->start();

    if (m_tableItemWidgetList.size() > 0) {
        this->hide();
    } else {
        this->show();
    }
}

MainWindow::~MainWindow() {
    m_timer->stop();
    while (m_scriptRunnerList.size() > 0) {
        m_scriptRunnerList[0]->quit();
        m_scriptRunnerList[0]->wait();
    }
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event) {
    hide();
    event->ignore();
}

void MainWindow::showEvent(QShowEvent *event) {
    ui->toolBar->setVisible(true);
    common::moveWidgetToCenter(this);
}

void MainWindow::_initTableWidget() {
    ui->tableWidget->clear();
    _clearTableWidgetAndData();

    ui->tableWidget->setColumnCount(m_tableHeaderLables.size());
    ui->tableWidget->horizontalHeader()->setVisible(true);
    ui->tableWidget->setHorizontalHeaderLabels(m_tableHeaderLables);
}

void MainWindow::_clearTableWidgetAndData() {
    while (ui->tableWidget->rowCount() > 0) {
        ui->tableWidget->removeRow(0);
    }
    m_tableItemWidgetList.clear();
}

void MainWindow::_initSystemTrayIcon() {
    if (QSystemTrayIcon::isSystemTrayAvailable()) {
        m_systemTrayIcon = new QSystemTrayIcon(this);
        m_systemTrayIcon->setIcon(QIcon(":/icons/info-white.png"));
        QMenu *trayMenu = new QMenu(this);
        QAction *showAction = new QAction("显示", m_systemTrayIcon);
        QAction *quitAction = new QAction("退出", m_systemTrayIcon);
        trayMenu->addAction(showAction);
        trayMenu->addAction(quitAction);
        m_systemTrayIcon->setContextMenu(trayMenu);
        connect(showAction, &QAction::triggered, this, &MainWindow::showNormal);
        connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
        connect(m_systemTrayIcon, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger) {
            if (isHidden()) {
                showNormal();
                activateWindow();
            } else {
                hide();
            }
        } });
        m_systemTrayIcon->show();
    } else {
        QMessageBox::critical(this, "报错", "当前系统不支持系统托盘，创建系统托盘失败");
        qApp->quit();
    }
}

void MainWindow::on_addOneRow() {
    QSharedPointer<OneTableRowItem> tableItemsPtr(new OneTableRowItem());
    _tableWidgetAddOneRow(tableItemsPtr);
}

void MainWindow::_tableWidgetAddOneRow(QSharedPointer<OneTableRowItem> tableItemsPtr) {
    m_tableItemWidgetList.append(tableItemsPtr);
    connect(tableItemsPtr.data(), &OneTableRowItem::editingFinished, this, &MainWindow::on_updateTableWidgetLayout);
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);
    for (int i = 0; i < ui->tableWidget->columnCount(); i++) {
        switch (i) {
        case 0: {
            QWidget *centeredWidget = new QWidget(ui->tableWidget);
            QHBoxLayout *layout = new QHBoxLayout(centeredWidget);
            layout->addWidget(tableItemsPtr->checkBox);
            layout->setAlignment(Qt::AlignCenter);  // 居中
            layout->setContentsMargins(0, 0, 0, 0); // 去除边距
            ui->tableWidget->setCellWidget(row, i, centeredWidget);
        } break;
        case 1: {
            ui->tableWidget->setCellWidget(row, i, tableItemsPtr->comBox);
        } break;
        case 2: {
            ui->tableWidget->setCellWidget(row, i, tableItemsPtr->dateTimeEdit);
        } break;
        case 3: {
            QWidget *centeredWidget = new QWidget(ui->tableWidget);
            QHBoxLayout *layout = new QHBoxLayout(centeredWidget);
            layout->addWidget(tableItemsPtr->lineEdit);
            layout->addWidget(tableItemsPtr->toolButton);
            layout->setContentsMargins(0, 0, 0, 0); // 去除边距
            layout->setSpacing(0);
            ui->tableWidget->setCellWidget(row, i, centeredWidget);
        } break;
        default:
            break;
        }
    }
    on_updateTableWidgetLayout();
}

void MainWindow::on_checkTime() {
    auto currentDateTime = QDateTime::currentDateTime();
    currentDateTime.setTime(QTime(currentDateTime.time().hour(), currentDateTime.time().minute(), currentDateTime.time().second()));
    ui->statusbar->showMessage(currentDateTime.toString(m_defaultDateTimeFormat));
    if (currentDateTime.time() == QTime(0, 0, 0)) {
        _updateDateTimeList();
    }
    for (auto pair : m_dateTimeList) {
        if (currentDateTime < pair.second) {
            break;
        } else if (currentDateTime == pair.second) {
            _showInfoWidget(pair.first);
        } else {
            continue;
        }
    }
}

void MainWindow::on_updateTableWidgetLayout() {
    for (int i = 0; i < ui->tableWidget->columnCount(); i++) {
        if (i == ui->tableWidget->columnCount() - 1) {
            ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::Stretch);
        } else {
            ui->tableWidget->horizontalHeader()->setSectionResizeMode(i, QHeaderView::ResizeToContents);
        }
    }
}

void MainWindow::_updateDateTimeList() {
    m_dateTimeList.clear();
    QDate currentDate = QDateTime::currentDateTime().date();
    for (int i = 0; i < m_tableItemWidgetList.size(); i++) {
        bool enable = m_tableItemWidgetList[i]->checkBox->isChecked();
        if (enable) {
            FrequencyType type = static_cast<FrequencyType>(m_tableItemWidgetList[i]->comBox->currentIndex());
            QDateTime targetDateTime = m_tableItemWidgetList[i]->dateTimeEdit->dateTime();
            QString str = m_tableItemWidgetList[i]->lineEdit->text();
            switch (type) {
            case FrequencyType::Once:
                break;
            case FrequencyType::EveryDay:
                targetDateTime.setDate(currentDate);
                break;
            case FrequencyType::EveryWeek: {
                int daysToAdd = targetDateTime.date().dayOfWeek() - currentDate.dayOfWeek();
                targetDateTime.setDate(currentDate.addDays(daysToAdd));
            } break;
            case FrequencyType::EveryMonth: {
                int targetDay = targetDateTime.date().day();
                if (targetDay >= targetDateTime.date().daysInMonth()) {
                    targetDateTime.setDate(QDate(currentDate.year(), currentDate.month(), currentDate.daysInMonth()));
                } else {
                    targetDateTime.setDate(QDate(currentDate.year(), currentDate.month(), targetDay));
                }
            } break;
            case FrequencyType::EveryYear:
                targetDateTime.setDate(QDate(currentDate.year(), targetDateTime.date().month(), targetDateTime.date().day()));
                break;
            default:
                break;
            }
            m_dateTimeList.append(QPair<int, QDateTime>(i, targetDateTime));
        }
    }
    std::sort(m_dateTimeList.begin(), m_dateTimeList.end(), [](const QPair<int, QDateTime> &a, const QPair<int, QDateTime> &b) {
        return a.second < b.second; // 按 QDateTime 升序排列
    });
}

void MainWindow::on_save() {
    _saveDataJsonFile();
    _updateDateTimeList();
    QMessageBox::information(this, "提示", "保存成功");
}

void MainWindow::_saveDataJsonFile() {
    QString currentPath = QApplication::applicationDirPath();
    QJsonObject rootObject;
    for (int i = 0; i < m_tableItemWidgetList.size(); i++) {
        QJsonObject object;
        object["enable"] = m_tableItemWidgetList[i]->checkBox->isChecked();
        object["type"] = m_tableItemWidgetList[i]->comBox->currentIndex();
        object["time"] = m_tableItemWidgetList[i]->dateTimeEdit->dateTime().toString(m_defaultDateTimeFormat);
        object["content"] = m_tableItemWidgetList[i]->lineEdit->text();
        rootObject[QString::number(i)] = object;
    }

    QFile file(m_saveDataFilePath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument jsonDoc(rootObject);
        file.write(jsonDoc.toJson());
        file.close();
    }
}
void MainWindow::_loadDataJsonFile() {
    _clearTableWidgetAndData();
    QFile file(m_saveDataFilePath);
    if (file.open(QIODevice::ReadOnly)) {
        QByteArray jsonData = file.readAll();
        file.close();

        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &jsonError);
        QJsonObject jsonObject = jsonDoc.object();
        for (int i = 0; i < jsonObject.keys().size(); i++) {
            auto key = jsonObject.keys()[i];
            auto valueDict = jsonObject[key].toObject();
            bool enable = valueDict["enable"].toBool();
            int type = valueDict["type"].toInt();
            QString timeStr = valueDict["time"].toString();
            QString content = valueDict["content"].toString();
            QSharedPointer<OneTableRowItem> tableItemsPtr(new OneTableRowItem(enable, type, QDateTime::fromString(timeStr, m_defaultDateTimeFormat), content));
            _tableWidgetAddOneRow(tableItemsPtr);
        }
        _updateDateTimeList();
    }
}

void MainWindow::_showInfoWidget(int index) {
    QString content = m_tableItemWidgetList[index]->lineEdit->text();
    auto contentList = content.split(" ", Qt::SkipEmptyParts);
    QPointer<ScriptRunner> _ScriptRunner = nullptr;
    if (contentList[0].endsWith(".py") ||
        contentList[0].endsWith(".sh")) {
        int _stopAfterSeconds = 0;
        int _waitStopSeconds = 30;
        if (contentList.size() == 3) {
            _stopAfterSeconds = contentList[1].toInt();
            _waitStopSeconds = contentList[2].toInt();
        }
        _ScriptRunner = new ScriptRunner(contentList[0], _stopAfterSeconds, _waitStopSeconds, this);
    }

    if (_ScriptRunner) {
        m_scriptRunnerList.append(_ScriptRunner);
        connect(_ScriptRunner.data(), &ScriptRunner::showInfo, this, &MainWindow::showTerminalInfo);
        connect(_ScriptRunner.data(), &ScriptRunner::finished, [this, _ScriptRunner]() {
            m_scriptRunnerList.removeOne(_ScriptRunner);
            _ScriptRunner->deleteLater();
        });
    } else {
        m_infoTextWidget->addInfo("[通知] " + QDateTime::currentDateTime().toString("yyyy/MM/dd-HH:mm:ss") + ": " + content);
        m_systemTrayIcon->showMessage("Notification", m_tableItemWidgetList[index]->lineEdit->text(), QSystemTrayIcon::Information, 10800);
    }
}

void MainWindow::on_delete() {
    auto res = QMessageBox::question(this, "删除", "是否确定删除?", QMessageBox::Yes, QMessageBox::No);
    if (res == QMessageBox::Yes) {
        auto ranges = ui->tableWidget->selectedRanges(); // 获取所有选中的单元格
        QSet<int> rowsToDelete;                          // 存储需要删除的行号（去重）
        for (const QTableWidgetSelectionRange &range : ranges) {
            for (int row = range.topRow(); row <= range.bottomRow(); ++row) {
                rowsToDelete.insert(row);
            }
        }

        QList<int> sortedRows = rowsToDelete.values();     // 转换为列表
        std::sort(sortedRows.rbegin(), sortedRows.rend()); // 按降序排序

        for (int row : sortedRows) {
            ui->tableWidget->removeRow(row);
            m_tableItemWidgetList.removeAt(row);
        }
    }
}

void MainWindow::showTerminalInfo(const QString &info) {
    if (ui->splitter->sizes()[1] == 0) {
        ui->splitter->setSizes({{1, 1}});
    }
    ui->plainTextEdit->appendPlainText(info);
}