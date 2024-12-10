#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ui_mainWindow.h"
#include <QAction>
#include <QCheckBox>
#include <QCloseEvent>
#include <QComboBox>
#include <QDate>
#include <QDateTimeEdit>
#include <QDebug>
#include <QFile>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QMessageBox>
#include <QSharedPointer>
#include <QShowEvent>
#include <QSystemTrayIcon>
#include <QTimer>
#include <QWidget>

#include "src/common/tools.h"
#include "src/components/oneRowTableItem.h"
#include "src/components/scriptRunner.h"
#include "src/widgets/infoTextWidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
    class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
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
    void _updateDateTimeList();
    void _loadDataJsonFile();
    void _saveDataJsonFile();
    void _clearTableWidgetAndData();
    void _tableWidgetAddOneRow(QSharedPointer<OneTableRowItem> tableItems);
    void _showInfoWidget(int index);

    void on_addOneRow();
    void on_save();
    void on_delete();
    void on_checkTime();
    void on_updateTableWidgetLayout();

private:
    Ui::MainWindow *ui;
    QTimer *m_timer = nullptr;
    QSystemTrayIcon *m_systemTrayIcon = nullptr;
    InfoTextWidget *m_infoTextWidget = nullptr;

    const QString m_saveDataFilePath = QApplication::applicationDirPath() + "/saveData.json";

    QList<QSharedPointer<OneTableRowItem>> m_tableItemWidgetList;
    QList<QPair<int, QDateTime>> m_dateTimeList;
};

#endif // MAINWINDOW_H
