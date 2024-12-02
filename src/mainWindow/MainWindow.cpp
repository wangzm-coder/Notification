#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _initTableWidget();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::_initTableWidget()
{
    ui->tableWidget->clear();
    while (ui->tableWidget->rowCount() > 0)
    {
        ui->tableWidget->removeRow(0);
    }
    QStringList labels = {"", "频率", "时间", "事件"};
    ui->tableWidget->setHorizontalHeaderLabels(labels);
}
