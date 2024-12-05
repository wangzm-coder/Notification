#include "oneRowTableItem.h"

OneTableRowItem::OneTableRowItem() : checkBox(new QCheckBox()),
                                     comBox(new QComboBox()),
                                     dateTimeEdit(new QDateTimeEdit()),
                                     lineEdit(new QLineEdit())
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QTime currentTime = currentDateTime.time();
    currentTime.setHMS(currentTime.hour(), currentTime.minute(), 0);
    currentDateTime.setTime(currentTime);
    _init(true, 0, currentDateTime, QString());
}

OneTableRowItem::OneTableRowItem(
    bool enable,
    int typeIndex,
    const QDateTime &dateTime,
    const QString &content) : checkBox(new QCheckBox()),
                              comBox(new QComboBox()),
                              dateTimeEdit(new QDateTimeEdit()),
                              lineEdit(new QLineEdit())
{
    _init(enable, typeIndex, dateTime, content);
}

OneTableRowItem::~OneTableRowItem() {}

void OneTableRowItem::_init(bool enable, int typeIndex, const QDateTime &dateTime, const QString &content)
{

    checkBox->setChecked(enable);
    lineEdit->setText(content);
    dateTimeEdit->setDateTime(dateTime);
    comBox->addItems(m_frequencyTypeMap.values());
    comBox->setCurrentIndex(typeIndex);

    connect(comBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index)
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
        dateTimeEdit->setDateTime(dateTimeEdit->dateTime());
        emit frequencyTypeChanged(); });

    comBox->currentIndexChanged(comBox->currentIndex());
}