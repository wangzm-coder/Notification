#pragma once

#include "ui_infoTextWidget.h"
#include <QCloseEvent>
#include <QHideEvent>
#include <QShowEvent>
#include <QTimer>
#include <QWidget>
#include <qscreen.h>

#include "src/common/tools.h"

class InfoTextWidget : public QWidget {
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
    Ui::InfoTextWidget *ui;
};
