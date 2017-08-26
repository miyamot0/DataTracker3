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

#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>

#include "sessionwindow.h"
#include "aboutdialog.h"
#include "contactdialog.h"

namespace Ui {
class StartWindow;
}

class StartWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = 0);
    ~StartWindow();

private slots:
    void on_actionExit_triggered();

    void on_actionCalculate_Reliability_triggered();

    void on_actionSettings_2_triggered();

    void on_actionContact_triggered();

    void on_actionAbout_triggered();

    void on_buttonStart_clicked();

private:
    Ui::StartWindow *ui;

    AboutDialog aboutDialogWindow;
    ContactDialog contactDialog;

    SessionWindow *sessionWindow;

    QString folderTitle;
    bool foundFolder = false;

    QString workingDirectory;


};

#endif // STARTWINDOW_H
