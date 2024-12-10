#include "oneRowTableItem.h"

OneTableRowItem::OneTableRowItem() {
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
    const QString &content) {
    _init(enable, typeIndex, dateTime, content);
}

OneTableRowItem::~OneTableRowItem() {}

void OneTableRowItem::_init(bool enable, int typeIndex, const QDateTime &dateTime, const QString &content) {

    checkBox = new QCheckBox();
    checkBox->setChecked(enable);

    dateTimeEdit = new QDateTimeEdit();
    dateTimeEdit->setDateTime(dateTime);

    lineEdit = new QLineEdit();
    lineEdit->setText(content);
    lineEdit->setPlaceholderText("事件/( 主动停止时间(s) 等待关闭时间(s) )");
    lineEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    connect(lineEdit, &QLineEdit::editingFinished, this, &OneTableRowItem::editingFinished);

    toolButton = new QToolButton();
    toolButton->setText("...");
    toolButton->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Preferred);
    connect(toolButton, &QToolButton::clicked, [this]() {
        QString fileName = QFileDialog::getOpenFileName(nullptr, "打开文件", QDir::homePath(), "所有文件 (*.*)");
        if (!fileName.isEmpty() && QFile::exists(fileName)) {
            lineEdit->setText(fileName);
        }
    });

    comBox = new QComboBox();
    comBox->addItems(m_frequencyTypeMap.values());
    comBox->setCurrentIndex(typeIndex);
    connect(comBox, QOverload<int>::of(&QComboBox::currentIndexChanged), [this](int index) {
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
        emit editingFinished(); });

    comBox->currentIndexChanged(comBox->currentIndex());
}