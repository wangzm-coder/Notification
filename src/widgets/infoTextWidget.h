#pragma once

#include <QWidget>
#include <QShowEvent>
#include <QHideEvent>
#include <QCloseEvent>
#include <qscreen.h>
#include <QTimer>
#include "ui_infoTextWidget.h"

#include "src/common/tools.h"

class InfoTextWidget : public QWidget
{
    Q_OBJECT
public:
    InfoTextWidget(QWidget *parent = nullptr);
    ~InfoTextWidget();

    void addInfo(const QString &msg);

protected:
    void showEvent(QShowEvent *event) override;
    void hideEvent(QHideEvent *event) override;
    void closeEvent(QCloseEvent *event) override;

private:
    void _moveToCenter();

private:
    Ui::InfoTextWidget *ui;
};
