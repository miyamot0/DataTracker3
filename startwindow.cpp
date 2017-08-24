#include "startwindow.h"
#include "ui_startwindow.h"

#include "sessionwindow.h"

#include <QDebug>

StartWindow::StartWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StartWindow)
{
    ui->setupUi(this);
}

StartWindow::~StartWindow()
{
    delete ui;
}

/** Close Window
 * @brief StartWindow::on_actionExit_triggered
 */
void StartWindow::on_actionExit_triggered()
{
    close();
}

/** Open Reli Window
 * @brief StartWindow::on_actionCalculate_Reliability_triggered
 */
void StartWindow::on_actionCalculate_Reliability_triggered()
{
    qDebug() << "on_actionCalculate_Reliability_triggered";
}

/** Open Settings Window
 * @brief StartWindow::on_actionSettings_2_triggered
 */
void StartWindow::on_actionSettings_2_triggered()
{
    qDebug() << "on_actionSettings_2_triggered";
}

/** Open Contact Window
 * @brief StartWindow::on_actionContact_triggered
 */
void StartWindow::on_actionContact_triggered()
{
    qDebug() << "on_actionContact_triggered";
}

/** Open About Window
 * @brief StartWindow::on_actionAbout_triggered
 */
void StartWindow::on_actionAbout_triggered()
{
    qDebug() << "on_actionAbout_triggered";
}

/** Open Session Window
 * @brief StartWindow::on_buttonStart_clicked
 */
void StartWindow::on_buttonStart_clicked()
{
    qDebug() << "on_buttonStart_clicked";

    SessionWindow *sessionWindow = new SessionWindow(this);
    sessionWindow->show();

}
