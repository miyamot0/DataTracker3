/**
   Copyright 2017 Shawn Gilroy

   This file is part of Data Tracker, Qt port.

   Discounting Model Selector is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 3.

   Discounting Model Selector is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Discounting Model Selector.  If not, see http://www.gnu.org/licenses/.

   The Discounting Model Selector is a tool to assist researchers in behavior economics.

   Email: shawn(dot)gilroy(at)temple.edu

  */

#include "startwindow.h"
#include "ui_startwindow.h"

#include "sessionwindow.h"
#include "filetools.h"

#include <QStandardPaths>
#include <QDir>

StartWindow::StartWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StartWindow)
{
    ui->setupUi(this);

    folderTitle = "DataTracker3";
    setWindowTitle(tr("Data Tracker"));

    if (FileTools::CheckAndPrepDirectory(folderTitle))
    {
        workingDirectory = FileTools::pathAppend(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0], folderTitle);
    }
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
    //qDebug() << "on_actionCalculate_Reliability_triggered";
}

/** Open Settings Window
 * @brief StartWindow::on_actionSettings_2_triggered
 */
void StartWindow::on_actionSettings_2_triggered()
{
    //qDebug() << "on_actionSettings_2_triggered";
}

/** Open Contact Window
 * @brief StartWindow::on_actionContact_triggered
 */
void StartWindow::on_actionContact_triggered()
{
    //qDebug() << "on_actionContact_triggered";
}

/** Open About Window
 * @brief StartWindow::on_actionAbout_triggered
 */
void StartWindow::on_actionAbout_triggered()
{
    //qDebug() << "on_actionAbout_triggered";
}

/** Open Session Window
 * @brief StartWindow::on_buttonStart_clicked
 */
void StartWindow::on_buttonStart_clicked()
{
    if (FileTools::CheckAndPrepDirectory(folderTitle))
    {
        sessionWindow = new SessionWindow(workingDirectory, this);
        sessionWindow->show();
    }
}
