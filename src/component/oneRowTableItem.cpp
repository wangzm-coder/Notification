#include "oneRowTableItem.h"

oneTableRowItem::oneTableRowItem() : checkBox(new QCheckBox()),
                                     comBox(new QComboBox()),
                                     dateTimeEdit(new QDateTimeEdit()),
                                     lineEdit(new QLineEdit())
{
    _init(true, 0, QString(), QString());
}

oneTableRowItem::oneTableRowItem(
    bool enable,
    int typeIndex,
    QString timeStr,
    QString content) : checkBox(new QCheckBox()),
                       comBox(new QComboBox()),
                       dateTimeEdit(new QDateTimeEdit()),
                       lineEdit(new QLineEdit())
{
    _init(enable, typeIndex, timeStr, content);
}

oneTableRowItem::~oneTableRowItem() {}

void oneTableRowItem::_init(bool enable, int typeIndex, QString timeStr, QString content)
{

    checkBox->setChecked(enable);
    lineEdit->setText(content);

    comBox->addItems(m_frequencyTypeMap.values());
    comBox->setCurrentIndex(typeIndex);
    if (timeStr.size() > 0)
    {
        dateTimeEdit->setDateTime(QDateTime::fromString(timeStr));
    }
    else
    {
        QDateTime currentDateTime = QDateTime::currentDateTime();
        QTime currentTime = currentDateTime.time();
        currentTime.setHMS(currentTime.hour(), currentTime.minute(), 0);
        currentDateTime.setTime(currentTime);
        dateTimeEdit->setDateTime(currentDateTime);
    }

    connect(comBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index)
            {
                        FrequencyType _type = static_cast<FrequencyType>(index);
                        switch (_type)
                        {
                        case FrequencyType::Once:
                            this->dateTimeEdit->setDisplayFormat("yyyy/MM/dd HH:mm:ss");
                            break;
                        case FrequencyType::EveryDay:
                            this->dateTimeEdit->setDisplayFormat("HH:mm:ss");
                            break;
                        case FrequencyType::EveryWeek:
                            this->dateTimeEdit->setDisplayFormat("dddd HH:mm:ss");
                            break;
                        case FrequencyType::EveryMonth:
                            this->dateTimeEdit->setDisplayFormat("/dd HH:mm:ss");
                            break;
                        case FrequencyType::EveryYear:
                            this->dateTimeEdit->setDisplayFormat("MM/dd HH:mm:ss");
                            break;
                        default:
                            break;
                        }
                        dateTimeEdit->setMinimumDateTime(QDateTime(QDate(1752, 9, 14), QTime(0, 0, 0)));
                        dateTimeEdit->setMaximumDateTime(QDateTime(QDate(9999, 12, 31), QTime(23, 59, 59)));
                        dateTimeEdit->setDateTime(dateTimeEdit->dateTime()); });
    comBox->currentIndexChanged(comBox->currentIndex());
}