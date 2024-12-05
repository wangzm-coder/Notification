#include "infoTextWidget.h"

InfoTextWidget::InfoTextWidget(QWidget *parent) : QWidget(parent),
                                                  ui(new Ui::InfoTextWidget)
{
    ui->setupUi(this);
    setWindowTitle("Notification");
    setWindowFlags(Qt::Window | Qt::WindowStaysOnTopHint | Qt::WindowCloseButtonHint);
    ui->plainTextEdit->setReadOnly(true);
}

InfoTextWidget::~InfoTextWidget()
{
    delete ui;
}

void InfoTextWidget::showEvent(QShowEvent *event)
{
    common::moveWidgetToCenter(this);
}

void InfoTextWidget::hideEvent(QHideEvent *event)
{
    ui->plainTextEdit->clear();
}

void InfoTextWidget::closeEvent(QCloseEvent *event)
{
    hide();
    event->ignore();
}

void InfoTextWidget::addInfo(const QString &msg)
{
    ui->plainTextEdit->appendPlainText(msg);
    show();
}