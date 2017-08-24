#include "sessionwindow.h"
#include "ui_sessionwindow.h"

SessionWindow::SessionWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SessionWindow)
{
    ui->setupUi(this);
}

SessionWindow::~SessionWindow()
{
    delete ui;
}
