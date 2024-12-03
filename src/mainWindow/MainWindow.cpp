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
    QStringList labels = {"", "频率", "时间", "事件"};
    ui->tableWidget->setColumnCount(labels.size());
    ui->tableWidget->horizontalHeader()->setVisible(true);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(0, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);
    ui->tableWidget->setHorizontalHeaderLabels(labels);
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

    QWidget *centeredWidget = new QWidget(ui->tableWidget);
    QCheckBox *checkBox = new QCheckBox(centeredWidget);
    QHBoxLayout *layout = new QHBoxLayout(centeredWidget);
    layout->addWidget(checkBox);
    layout->setAlignment(Qt::AlignCenter);  // 居中
    layout->setContentsMargins(0, 0, 0, 0); // 去除边距
    checkBox->setChecked(true);

    QComboBox *comBox = new QComboBox(ui->tableWidget);
    comBox->addItems(QStringList(m_frequencyMap.values()));

    QDateTimeEdit *dateTimeEdit = new QDateTimeEdit(ui->tableWidget);

    QLineEdit *lineEdit = new QLineEdit(ui->tableWidget);

    connect(comBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this, dateTimeEdit](int index)
            {
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

    comBox->currentIndexChanged(comBox->currentIndex());

    ui->tableWidget->setCellWidget(row, 0, centeredWidget);
    ui->tableWidget->setCellWidget(row, 1, comBox);
    ui->tableWidget->setCellWidget(row, 2, dateTimeEdit);
    ui->tableWidget->setCellWidget(row, 3, lineEdit);
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
    for (auto it = m_tableDataMap.begin(); it != m_tableDataMap.end(); it++)
    {
        QList<QVariant> varList = it.value();
        bool enable = varList[0].toBool();
        if (enable)
        {
            FrequencyType type = static_cast<FrequencyType>(varList[1].toInt());
            QDateTime targetDateTime = varList[2].toDateTime();
            QString str = varList[3].toString();
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
            m_dateTimeList.append(QPair<int, QDateTime>(it.key(), targetDateTime));
        }
    }
    std::sort(m_dateTimeList.begin(), m_dateTimeList.end(), [](const QPair<int, QDateTime> &a, const QPair<int, QDateTime> &b)
              {
                  return a.second < b.second; // 按 QDateTime 升序排列
              });
}

void MainWindow::on_save()
{
    m_tableDataMap.clear();
    QWidget *widget = nullptr;
    for (int i = 0; i < ui->tableWidget->rowCount(); i++)
    {
        QList<QVariant> varList;
        widget = ui->tableWidget->cellWidget(i, 0);
        varList.append(qobject_cast<QCheckBox *>(widget->layout()->widget())->isChecked());

        widget = ui->tableWidget->cellWidget(i, 1);
        varList.append(qobject_cast<QComboBox *>(widget)->currentIndex());

        widget = ui->tableWidget->cellWidget(i, 2);
        varList.append(qobject_cast<QDateTimeEdit *>(widget)->dateTime());

        widget = ui->tableWidget->cellWidget(i, 3);
        varList.append(qobject_cast<QLineEdit *>(widget)->text());

        m_tableDataMap[i] = varList;
    }
    _updateDateTimeList();
    if (m_tableDataMap.size() > 0)
    {
        QMessageBox::information(this, "提示", "保存成功");
    }
}

void MainWindow::_showInfoWidget(int index)
{
    QString infoText;
    if (m_tableDataMap.contains(index))
    {
        auto items = m_tableDataMap[index];
        infoText = items[items.size() - 1].toString();
    }
}

void MainWindow::on_delete()
{
}