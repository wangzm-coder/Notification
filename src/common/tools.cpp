#include "tools.h"

void common::moveWidgetToCenter(QWidget *widget)
{
    QScreen *screen = QApplication::primaryScreen();
    widget->setGeometry((screen->geometry().width() - widget->width()) / 2, (screen->geometry().height() - widget->height()) / 2, widget->width(), widget->height());
    widget->activateWindow();
}