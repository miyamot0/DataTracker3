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

#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

namespace Ui {
class SettingsDialog;
}

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    QString primarySaveLocation;
    QString alternateSaveLocation;
    bool spreadsheetOutput = true;
    bool displayPlots = false;
    bool displayDark = false;
    bool autoReli = false;
    bool autoMigrate = false;
    bool autoUpdate = true;

    void SetPrimarySaveLocation(QString location);
    void SetSaveLocation(QString location);
    void SetSpreadsheetOption(bool value);
    void SetDisplayOption(bool value);
    void SetThemeDark(bool value);
    void SetAutoReli(bool value);
    void SetAutoMigrate(bool value);
    void SetAutoUpdateCheck(bool value);

    QString GetPrimarySaveLocation();
    QString GetSaveLocation();
    bool GetSpreadsheetOption();
    bool GetDisplayOption();
    bool GetThemeDark();
    bool GetAutoReli();
    bool GetAutoMigrate();
    bool GetAutoUpdate();

    ~SettingsDialog();

private slots:
    void on_pushButton_clicked();
    void on_setSaveLocation_clicked();
    void on_setPrimarySaveLocation_clicked();

private:
    Ui::SettingsDialog *ui;
};

#endif // SETTINGSDIALOG_H
