/**
   Copyright 2017 Shawn Gilroy

   This file is part of Data Tracker, Qt port.

   Data Tracker is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 3.

   Data Tracker is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Data Tracker.  If not, see http://www.gnu.org/licenses/.

   The Data Tracker is a tool to assist researchers in behavior economics.

   Email: shawn(dot)gilroy(at)temple.edu

  */

#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>

#include "sessionwindow.h"
#include "reliabilitydialog.h"
#include "settingsdialog.h"
#include "aboutdialog.h"
#include "contactdialog.h"
#include "licensedialog.h"

namespace Ui {
class StartWindow;
}

class StartWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = 0);
    ~StartWindow();

    void SaveSettings(QString savedLocation, bool plots, bool dark, bool sheets);
    void LoadSettings();

private slots:
    void on_actionExit_triggered();
    void on_actionCalculate_Reliability_triggered();
    void on_actionSettings_2_triggered();
    void on_actionQt_Framework_triggered();
    void on_actionQtXlsx_triggered();
    void on_actionTango_Icons_triggered();
    void on_actionQDarkStyleSheet_triggered();
    void on_actionContact_triggered();
    void on_actionAbout_triggered();
    void on_buttonStart_clicked();

private:
    Ui::StartWindow *ui;

    AboutDialog aboutDialogWindow;
    ContactDialog contactDialog;
    LicenseDialog *licenseDialog;
    ReliabilityDialog reliabilityDialog;
    SettingsDialog settingsDialog;

    SessionWindow *sessionWindow;

    QString folderTitle;
    bool foundFolder = false;

    QString workingDirectory;
    QString backupSaveLocation;
    bool displayPlots;
    bool displayDark;
    bool outputSheets;

    void closeEvent(QCloseEvent*);

};

#endif // STARTWINDOW_H
