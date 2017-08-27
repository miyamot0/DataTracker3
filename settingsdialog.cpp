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

#include "settingsdialog.h"
#include "ui_settingsdialog.h"
#include "filetools.h"

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->editSaveLocation->installEventFilter(this);

    setWindowTitle(tr("Program Settings"));
}

/**
 * @brief SettingsDialog::eventFilter
 * @param obj
 * @param e
 * @return
 */
bool SettingsDialog::eventFilter(QObject *obj, QEvent *e)
{
    if (e->type() == QEvent::FocusIn)
    {
        if (obj == ui->editSaveLocation)
        {
            QString mLocation;

            if (ui->editSaveLocation->text().length() == 0)
            {
                mLocation = QFileDialog::getExistingDirectory (this,
                                                               tr("Pick Alternate Save Location"),
                                                               QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0],
                                                               QFileDialog::ShowDirsOnly);
            }
            else
            {
                mLocation = QFileDialog::getExistingDirectory (this,
                                                               tr("Pick Alternate Save Location"),
                                                               ui->editSaveLocation->text(),
                                                               QFileDialog::ShowDirsOnly);
            }

            QDir mPotentialDir(mLocation);

            if (mLocation.length() !=0 && mPotentialDir.exists())
            {
                ui->editSaveLocation->setText(mLocation);
            }
        }

        ui->pushButton->setFocus();
        ui->editSaveLocation->clearFocus();
        ui->pushButton->setFocus();
    }

    return false;
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

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

/**
 * @brief SettingsDialog::on_pushButton_clicked
 */
void SettingsDialog::on_pushButton_clicked()
{
    alternateSaveLocation = ui->editSaveLocation->text();
    spreadsheetOutput = ui->checkBoxSheets->isChecked();
    displayPlots = ui->checkBoxGraphs->isChecked();
    displayDark = (ui->comboTheme->currentIndex() == 1);

    QDir mDir(alternateSaveLocation);

    if (mDir.exists())
    {
        FileTools::CheckAndPrepDirectory("DataTracker3", mDir.absolutePath());
    }

    accept();
}
