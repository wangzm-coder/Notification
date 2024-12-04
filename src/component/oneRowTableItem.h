#pragma once
#include <QMap>
#include <QCheckBox>
#include <QComboBox>
#include <QDateTimeEdit>
#include <QDateTime>
#include <QTime>
#include <QLineEdit>
#include <QObject>

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

class oneTableRowItem : public QObject
{
    Q_OBJECT
public:
    QCheckBox *checkBox = nullptr;
    QComboBox *comBox = nullptr;
    QDateTimeEdit *dateTimeEdit = nullptr;
    QLineEdit *lineEdit = nullptr;

    oneTableRowItem();
    oneTableRowItem(bool enable, int typeIndex, QString timeStr, QString content);
    ~oneTableRowItem();

    oneTableRowItem(const oneTableRowItem &) = delete;
    oneTableRowItem &operator=(const oneTableRowItem &) = delete;

private:
    void _init(bool enable = true, int typeIndex = 0, QString timeStr = QString(), QString content = QString());
};
