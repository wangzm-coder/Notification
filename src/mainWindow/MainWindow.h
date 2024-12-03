#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QWidget>
#include <QHBoxLayout>
#include <QCloseEvent>
#include <QShowEvent>
#include <QLineEdit>
#include <QMessageBox>
#include <QMenu>
#include <QDebug>
#include <QAction>
#include <QTimer>
#include <QSystemTrayIcon>
#include "ui_MainWindow.h"

QT_BEGIN_NAMESPACE
namespace Ui
{
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void closeEvent(QCloseEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    void _initTableWidget();
    void _initSystemTrayIcon();

    void on_addOneRow();
    void on_save();
    void on_delete();
    void on_checkTime();

private:
    Ui::MainWindow *ui;
    QTimer *m_timer = nullptr;
    QSystemTrayIcon *m_systemTrayIcon = nullptr;

    enum class FrequencyType
    {
        Once = 0,
        EveryDay,
        EveryWeek,
        EveryMonth,
        EveryYear
    };
    QMap<FrequencyType, QString> m_frequencyMap = {{FrequencyType::Once, "单次"},
                                                   {FrequencyType::EveryDay, "每天"},
                                                   {FrequencyType::EveryWeek, "每周"},
                                                   {FrequencyType::EveryMonth, "每月"},
                                                   {FrequencyType::EveryYear, "每年"}};
    QMap<int, QList<QVariant>> m_tableDataMap;
};
#endif // MAINWINDOW_H
