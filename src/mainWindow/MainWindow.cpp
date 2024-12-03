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
    ui->tableWidget->clear();
    while (ui->tableWidget->rowCount() > 0)
    {
        ui->tableWidget->removeRow(0);
    }
    ui->tableWidget->setColumnCount(m_tableHeaderLables.size());
    ui->tableWidget->horizontalHeader()->setVisible(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->tableWidget->setHorizontalHeaderLabels(m_tableHeaderLables);
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
    int row = ui->tableWidget->rowCount();
    ui->tableWidget->insertRow(row);
    oneTableRowItem tableItems;
    for (int i = 0; i < ui->tableWidget->columnCount(); i++)
    {
        switch (i)
        {
        case 0:
        {
            QWidget *centeredWidget = new QWidget(ui->tableWidget);
            tableItems.checkBox = new QCheckBox(centeredWidget);
            QHBoxLayout *layout = new QHBoxLayout(centeredWidget);
            layout->addWidget(tableItems.checkBox);
            layout->setAlignment(Qt::AlignCenter);  // 居中
            layout->setContentsMargins(0, 0, 0, 0); // 去除边距
            tableItems.checkBox->setChecked(true);
            ui->tableWidget->setCellWidget(row, i, centeredWidget);
        }
        break;
        case 1:
        {
            tableItems.comBox = new QComboBox(ui->tableWidget);
            tableItems.comBox->addItems(QStringList(m_frequencyTypeMap.values()));
            ui->tableWidget->setCellWidget(row, i, tableItems.comBox);

            connect(tableItems.comBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, row](int index)
                    {
                auto dateTimeEdit=this->m_tableItemWidgetList[row].dateTimeEdit;
                FrequencyType _type = static_cast<FrequencyType>(index);
                switch (_type)
                {
                case FrequencyType::Once:
                    dateTimeEdit->setDisplayFormat("yyyy/MM/dd HH:mm:ss");
                    break;
                case FrequencyType::EveryDay:
                    dateTimeEdit->setDisplayFormat("HH:mm:ss");
                    break;
                case FrequencyType::EveryWeek:
                    dateTimeEdit->setDisplayFormat("dddd HH:mm:ss");
                    break;
                case FrequencyType::EveryMonth:
                    dateTimeEdit->setDisplayFormat("/dd HH:mm:ss");
                    break;
                case FrequencyType::EveryYear:
                    dateTimeEdit->setDisplayFormat("MM/dd HH:mm:ss");
                    break;
                default:
                    break;
                }
                dateTimeEdit->setMinimumDateTime(QDateTime(QDate(1752, 9, 14), QTime(0, 0, 0)));
                dateTimeEdit->setMaximumDateTime(QDateTime(QDate(9999, 12, 31), QTime(23, 59, 59)));
                QDateTime currentDateTime = QDateTime::currentDateTime();
                QTime currentTime = currentDateTime.time();
                currentTime.setHMS(currentTime.hour(), currentTime.minute(), 0);
                currentDateTime.setTime(currentTime);
                dateTimeEdit->setDateTime(currentDateTime); });
        }
        break;
        case 2:
        {
            tableItems.dateTimeEdit = new QDateTimeEdit(ui->tableWidget);
            ui->tableWidget->setCellWidget(row, i, tableItems.dateTimeEdit);
        }
        break;
        case 3:
        {
            tableItems.lineEdit = new QLineEdit(ui->tableWidget);
            ui->tableWidget->setCellWidget(row, i, tableItems.lineEdit);
        }
        break;
        default:
            break;
        }
    }
    m_tableItemWidgetList.append(tableItems);
    tableItems.comBox->currentIndexChanged(tableItems.comBox->currentIndex());
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
        bool enable = m_tableItemWidgetList[i].checkBox->isChecked();
        if (enable)
        {
            FrequencyType type = static_cast<FrequencyType>(m_tableItemWidgetList[i].comBox->currentIndex());
            QDateTime targetDateTime = m_tableItemWidgetList[i].dateTimeEdit->dateTime();
            QString str = m_tableItemWidgetList[i].lineEdit->text();
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
    _updateDateTimeList();
    QMessageBox::information(this, "提示", "保存成功");
}

void MainWindow::_showInfoWidget(int index)
{
    QString infoText = m_tableItemWidgetList[index].lineEdit->text();
}

void MainWindow::on_delete()
{
}