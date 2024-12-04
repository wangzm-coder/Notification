#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_timer(new QTimer(this))
{
    ui->setupUi(this);
    _initSystemTrayIcon();
    _initTableWidget();
    setWindowIcon(QIcon(":/icons/notification.png"));
    setWindowTitle("Notification");

    connect(ui->action_add, &QAction::triggered, this, &MainWindow::on_addOneRow);
    connect(ui->action_save, &QAction::triggered, this, &MainWindow::on_save);
    connect(ui->action_delete, &QAction::triggered, this, &MainWindow::on_delete);

    m_timer->setInterval(1000);
    connect(m_timer, &QTimer::timeout, this, &MainWindow::on_checkTime);
    m_timer->start();

    _loadDataJsonFile();
}

MainWindow::~MainWindow()
{
    m_timer->stop();
    delete ui;
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

void MainWindow::showEvent(QShowEvent *event)
{
    ui->toolBar->setVisible(true);
}

void MainWindow::_initTableWidget()
{
    _clearTableWidgetAndData();
    ui->tableWidget->setColumnCount(m_tableHeaderLables.size());
    ui->tableWidget->horizontalHeader()->setVisible(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->tableWidget->setHorizontalHeaderLabels(m_tableHeaderLables);
}

void MainWindow::_clearTableWidgetAndData()
{
    ui->tableWidget->clear();
    while (ui->tableWidget->rowCount() > 0)
    {
        ui->tableWidget->removeRow(0);
    }
    m_tableItemWidgetList.clear();
}

void MainWindow::_initSystemTrayIcon()
{
    if (QSystemTrayIcon::isSystemTrayAvailable())
    {
        m_systemTrayIcon = new QSystemTrayIcon(this);
        m_systemTrayIcon->setIcon(QIcon(":/icons/notification.png"));
        QMenu *trayMenu = new QMenu(this);
        QAction *showAction = new QAction("显示", m_systemTrayIcon);
        QAction *quitAction = new QAction("退出", m_systemTrayIcon);
        trayMenu->addAction(showAction);
        trayMenu->addAction(quitAction);
        m_systemTrayIcon->setContextMenu(trayMenu);
        connect(showAction, &QAction::triggered, this, &MainWindow::showNormal);
        connect(quitAction, &QAction::triggered, qApp, &QApplication::quit);
        connect(m_systemTrayIcon, &QSystemTrayIcon::activated, [this](QSystemTrayIcon::ActivationReason reason)
                {
        if (reason == QSystemTrayIcon::Trigger) {
            if (isHidden()) {
                showNormal();
                activateWindow();
            } else {
                hide();
            }
        } });
        m_systemTrayIcon->show();
    }
    else
    {
        QMessageBox::critical(this, "报错", "当前系统不支持系统托盘，创建系统托盘失败");
        qApp->quit();
    }
}

void MainWindow::on_addOneRow()
{
    QSharedPointer<oneTableRowItem> tableItemsPtr(new oneTableRowItem());
    _tableWidgetAddOneRow(tableItemsPtr);
}

void MainWindow::_tableWidgetAddOneRow(QSharedPointer<oneTableRowItem> tableItemsPtr)
{
    m_tableItemWidgetList.append(tableItemsPtr);
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);
    for (int i = 0; i < ui->tableWidget->columnCount(); i++)
    {
        switch (i)
        {
        case 0:
        {
            QWidget *centeredWidget = new QWidget(ui->tableWidget);
            QHBoxLayout *layout = new QHBoxLayout(centeredWidget);
            layout->addWidget(tableItemsPtr->checkBox);
            layout->setAlignment(Qt::AlignCenter);  // 居中
            layout->setContentsMargins(0, 0, 0, 0); // 去除边距
            ui->tableWidget->setCellWidget(row, i, centeredWidget);
        }
        break;
        case 1:
        {
            ui->tableWidget->setCellWidget(row, i, tableItemsPtr->comBox);
        }
        break;
        case 2:
        {
            ui->tableWidget->setCellWidget(row, i, tableItemsPtr->dateTimeEdit);
        }
        break;
        case 3:
        {
            ui->tableWidget->setCellWidget(row, i, tableItemsPtr->lineEdit);
        }
        break;
        default:
            break;
        }
    }
}

void MainWindow::on_checkTime()
{
    auto currentDateTime = QDateTime::currentDateTime();
    ui->statusbar->showMessage(currentDateTime.toString("yyyy/MM/dd dddd HH:mm:ss"));
    if (currentDateTime.time() == QTime(0, 0, 0))
    {
        _updateDateTimeList();
    }
    for (auto pair : m_dateTimeList)
    {
        if (currentDateTime < pair.second)
        {
            continue;
        }
        else if (currentDateTime == pair.second)
        {
            _showInfoWidget(pair.first);
        }
        else
        {
            break;
        }
    }
}

void MainWindow::_updateDateTimeList()
{
    m_dateTimeList.clear();
    QDate currentDate = QDateTime::currentDateTime().date();
    for (int i = 0; i < m_tableItemWidgetList.size(); i++)
    {
        bool enable = m_tableItemWidgetList[i]->checkBox->isChecked();
        if (enable)
        {
            FrequencyType type = static_cast<FrequencyType>(m_tableItemWidgetList[i]->comBox->currentIndex());
            QDateTime targetDateTime = m_tableItemWidgetList[i]->dateTimeEdit->dateTime();
            QString str = m_tableItemWidgetList[i]->lineEdit->text();
            switch (type)
            {
            case FrequencyType::Once:
                break;
            case FrequencyType::EveryDay:
                targetDateTime.setDate(currentDate);
                break;
            case FrequencyType::EveryWeek:
            {
                int daysToAdd = targetDateTime.date().dayOfWeek() - currentDate.dayOfWeek();
                targetDateTime.setDate(currentDate.addDays(daysToAdd));
            }
            break;
            case FrequencyType::EveryMonth:
            {
                int targetDay = targetDateTime.date().day();
                if (targetDay >= targetDateTime.date().daysInMonth())
                {
                    targetDateTime.setDate(QDate(currentDate.year(), currentDate.month(), currentDate.daysInMonth()));
                }
                else
                {
                    targetDateTime.setDate(QDate(currentDate.year(), currentDate.month(), targetDay));
                }
            }
            break;
            case FrequencyType::EveryYear:
                targetDateTime.setDate(QDate(currentDate.year(), targetDateTime.date().month(), targetDateTime.date().day()));
                break;
            default:
                break;
            }
            m_dateTimeList.append(QPair<int, QDateTime>(i, targetDateTime));
        }
    }
    std::sort(m_dateTimeList.begin(), m_dateTimeList.end(), [](const QPair<int, QDateTime> &a, const QPair<int, QDateTime> &b)
              {
                  return a.second < b.second; // 按 QDateTime 升序排列
              });
}

void MainWindow::on_save()
{
    _saveDataJsonFile();
    _updateDateTimeList();
    QMessageBox::information(this, "提示", "保存成功");
}

void MainWindow::_saveDataJsonFile()
{
    QString currentPath = QApplication::applicationDirPath();
    QJsonObject rootObject;
    for (int i = 0; i < m_tableItemWidgetList.size(); i++)
    {
        QJsonObject object;
        object["enable"] = m_tableItemWidgetList[i]->checkBox->isChecked();
        object["type"] = m_tableItemWidgetList[i]->comBox->currentIndex();
        object["time"] = m_tableItemWidgetList[i]->dateTimeEdit->dateTime().toString("yyyy/MM/dd HH:mm:ss");
        object["content"] = m_tableItemWidgetList[i]->lineEdit->text();
        rootObject[QString::number(i)] = object;
    }

    QFile file(m_saveDataFilePath);
    if (file.open(QIODevice::WriteOnly))
    {
        QJsonDocument jsonDoc(rootObject);
        file.write(jsonDoc.toJson());
        file.close();
    }
}
void MainWindow::_loadDataJsonFile()
{
    _clearTableWidgetAndData();
    QFile file(m_saveDataFilePath);
    if (file.open(QIODevice::ReadOnly))
    {
        QByteArray jsonData = file.readAll();
        file.close();

        QJsonParseError jsonError;
        QJsonDocument jsonDoc = QJsonDocument::fromJson(jsonData, &jsonError);
        QJsonObject jsonObject = jsonDoc.object();
        for (int i = 0; i < jsonObject.keys().size(); i++)
        {
            auto key = jsonObject.keys()[i];
            auto valueDict = jsonObject[key].toObject();
            bool enable = valueDict["enable"].toBool();
            int type = valueDict["type"].toInt();
            QString timeStr = valueDict["time"].toString();
            QString content = valueDict["content"].toString();
            QSharedPointer<oneTableRowItem> tableItemsPtr(new oneTableRowItem(enable, type, timeStr, content));
            _tableWidgetAddOneRow(tableItemsPtr);
        }
        _updateDateTimeList();
    }
}

void MainWindow::_showInfoWidget(int index)
{
    QString infoText = m_tableItemWidgetList[index]->lineEdit->text();
}

void MainWindow::on_delete()
{
}