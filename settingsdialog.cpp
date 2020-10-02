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

#include <QFileDialog>
#include <QStandardPaths>
#include <QMessageBox>

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "filetools.h"
#include "windowtools.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("Program Settings"));

    WindowTools::SetDialogFixedDisplay(this);
}

/**
 * @brief SettingsDialog::SetPrimarySaveLocation
 * @param location
 */
void SettingsDialog::SetPrimarySaveLocation(QString location)
{
    primarySaveLocation = location;
    ui->editPrimarySaveLocation->setText(location);
}

/**
 * @brief SettingsDialog::SetSaveLocation
 * @param location
 */
void SettingsDialog::SetSaveLocation(QString location)
{
    alternateSaveLocation = location;
    ui->editSaveLocation->setText(location);
}

/**
 * @brief SettingsDialog::SetSpreadsheetOption
 * @param value
 */
void SettingsDialog::SetSpreadsheetOption(bool value)
{
    spreadsheetOutput = value;
    ui->checkBoxSheets->setChecked(value);
}

/**
 * @brief SettingsDialog::SetDisplayOption
 * @param value
 */
void SettingsDialog::SetDisplayOption(bool value)
{
    displayPlots = value;
    ui->checkBoxGraphs->setChecked(value);
}

/**
 * @brief SettingsDialog::SetThemeDark
 * @param value
 */
void SettingsDialog::SetThemeDark(bool value)
{
    displayDark = value;

    if (value)
    {
        ui->comboTheme->setCurrentIndex(1);
    }
    else
    {
        ui->comboTheme->setCurrentIndex(0);
    }
}

/**
 * @brief SettingsDialog::SetAutoReli
 * @param value
 * @return
 */
void SettingsDialog::SetAutoReli(bool value)
{
    autoReli = value;
    ui->checkBoxReli->setChecked(value);
}

/**
 * @brief SettingsDialog::SetAutoMigrate
 * @param value
 */
void SettingsDialog::SetAutoMigrate(bool value)
{
    autoMigrate = value;
    ui->checkBoxAutoMigrate->setChecked(value);
}

/**
 * @brief SettingsDialog::SetAutoUpdateCheck
 * @param value
 */
void SettingsDialog::SetAutoUpdateCheck(bool value)
{
    autoUpdate = value;
    ui->checkBoxAutoUpdate->setChecked(value);
}

/**
 * @brief SettingsDialog::GetPrimarySaveLocation
 * @return
 */
QString SettingsDialog::GetPrimarySaveLocation()
{
    return ui->editPrimarySaveLocation->text();
}

/**
 * @brief SettingsDialog::GetSaveLocation
 * @return
 */
QString SettingsDialog::GetSaveLocation()
{
    return ui->editSaveLocation->text();
}

/**
 * @brief SettingsDialog::GetSpreadsheetOption
 * @return
 */
bool SettingsDialog::GetSpreadsheetOption()
{
    return ui->checkBoxSheets->isChecked();
}

/**
 * @brief SettingsDialog::GetDisplayOption
 * @return
 */
bool SettingsDialog::GetDisplayOption()
{
    return ui->checkBoxGraphs->isChecked();
}

/**
 * @brief SettingsDialog::GetThemeDark
 * @return
 */
bool SettingsDialog::GetThemeDark()
{
    return (ui->comboTheme->currentIndex() == 1);
}

/**
 * @brief SettingsDialog::GetAutoReli
 * @return
 */
bool SettingsDialog::GetAutoReli()
{
    return (ui->checkBoxReli->isChecked());
}

/**
 * @brief SettingsDialog::GetAutoMigrate
 * @return
 */
bool SettingsDialog::GetAutoMigrate()
{
    return (ui->checkBoxAutoMigrate->isChecked());
}

/**
 * @brief SettingsDialog::GetAutoUpdate
 * @return
 */
bool SettingsDialog::GetAutoUpdate()
{
    return (ui->checkBoxAutoUpdate->isChecked());
}

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

/**
 * @brief SettingsDialog::on_pushButton_clicked
 */
void SettingsDialog::on_pushButton_clicked()
{
    primarySaveLocation = GetPrimarySaveLocation();
    alternateSaveLocation = GetSaveLocation();
    spreadsheetOutput = GetSpreadsheetOption();
    displayPlots = GetDisplayOption();
    displayDark = GetDisplayOption();
    autoReli = GetAutoReli();
    autoMigrate = GetAutoMigrate();
    autoUpdate = GetAutoUpdate();

    QDir mDir(primarySaveLocation);
    if (mDir.exists())
    {
        FileTools::CheckAndPrepDirectory("DataTracker3", mDir.absolutePath());
    }

    mDir = QDir(alternateSaveLocation);
    if (mDir.exists())
    {
        FileTools::CheckAndPrepDirectory("DataTracker3", mDir.absolutePath());
    }

    accept();
}

void SettingsDialog::on_setSaveLocation_clicked()
{
    QString mLocation;

    if (ui->editSaveLocation->text().length() == 0)
    {
#ifdef _WIN32
        mLocation = QFileDialog::getExistingDirectory (this,
                                                       tr("Pick Alternate Save Location"),
                                                       QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0],
                                                       QFileDialog::ShowDirsOnly);
#elif TARGET_OS_MAC
        mLocation = QFileDialog::getExistingDirectory (this,
                                                       tr("Pick Alternate Save Location"),
                                                       QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0],
                                                       QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);
#endif
    }
    else
    {
#ifdef _WIN32
        mLocation = QFileDialog::getExistingDirectory (this,
                                                       tr("Pick Alternate Save Location"),
                                                       ui->editSaveLocation->text(),
                                                       QFileDialog::ShowDirsOnly);
#elif TARGET_OS_MAC
        mLocation = QFileDialog::getExistingDirectory (this,
                                                       tr("Pick Alternate Save Location"),
                                                       ui->editSaveLocation->text(),
                                                       QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);
#endif
    }

    if (mLocation.contains("DataTracker3", Qt::CaseInsensitive))
    {
        mLocation = mLocation.replace("DataTracker3", "");
    }

    QDir mPotentialDir(mLocation);

    if (mLocation.length() !=0 && mPotentialDir.exists())
    {
        ui->editSaveLocation->setText(mLocation);
    }
}

void SettingsDialog::on_setPrimarySaveLocation_clicked()
{
    QString mLocation;

    if (ui->editPrimarySaveLocation->text().length() == 0)
    {
#ifdef _WIN32
        mLocation = QFileDialog::getExistingDirectory (this,
                                                       tr("Pick Primary Save Location"),
                                                       QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0],
                                                       QFileDialog::ShowDirsOnly);
#elif TARGET_OS_MAC
        mLocation = QFileDialog::getExistingDirectory (this,
                                                       tr("Pick Primary Save Location"),
                                                       QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0],
                                                       QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);
#endif
    }
    else
    {
#ifdef _WIN32
        mLocation = QFileDialog::getExistingDirectory (this,
                                                       tr("Pick Primary Save Location"),
                                                       ui->editPrimarySaveLocation->text(),
                                                       QFileDialog::ShowDirsOnly);
#elif TARGET_OS_MAC
        mLocation = QFileDialog::getExistingDirectory (this,
                                                       tr("Pick Primary Save Location"),
                                                       ui->editPrimarySaveLocation->text(),
                                                       QFileDialog::ShowDirsOnly | QFileDialog::DontUseNativeDialog);
#endif
    }

    // Remove the trailing location, if added
    if (mLocation.contains("DataTracker3", Qt::CaseInsensitive))
    {
        mLocation = mLocation.replace("DataTracker3", "");
    }

    QDir mPotentialDir(mLocation);
    if (mLocation.length() !=0 && mPotentialDir.exists())
    {
        ui->editPrimarySaveLocation->setText(mLocation);
    }
}
