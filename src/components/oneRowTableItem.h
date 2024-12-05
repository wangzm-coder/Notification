#pragma once

#include <QMap>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QDateTime>
#include <QTime>
#include <QLineEdit>
#include <QTableWidget>
#include <QObject>
#include <QHeaderView>

enum class FrequencyType
{
    Once = 0,
    EveryDay,
    EveryWeek,
    EveryMonth,
    EveryYear
};

const QMap<FrequencyType, QString> m_frequencyTypeMap = {{FrequencyType::Once, "单次"},
                                                         {FrequencyType::EveryDay, "每天"},
                                                         {FrequencyType::EveryWeek, "每周"},
                                                         {FrequencyType::EveryMonth, "每月"},
                                                         {FrequencyType::EveryYear, "每年"}};

const QStringList m_tableHeaderLables = {QString(), "频率", "时间", "事件"};
const QString m_defaultDateTimeFormat = "yyyy/MM/dd dddd HH:mm:ss";

class OneTableRowItem : public QObject
{
    Q_OBJECT
public:
    QCheckBox *checkBox = nullptr;
    QComboBox *comBox = nullptr;
    QDateTimeEdit *dateTimeEdit = nullptr;
    QLineEdit *lineEdit = nullptr;

    OneTableRowItem();
    OneTableRowItem(bool enable, int typeIndex, const QDateTime &dateTime, const QString &content);
    ~OneTableRowItem();

    OneTableRowItem(const OneTableRowItem &) = delete;
    OneTableRowItem &operator=(const OneTableRowItem &) = delete;

signals:
    void frequencyTypeChanged();

private:
    void _init(bool enable, int typeIndex, const QDateTime &dateTime, const QString &content);
    void _updateContainerWidgetLayout();
};
