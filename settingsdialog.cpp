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

SettingsDialog::SettingsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SettingsDialog)
{
    ui->setupUi(this);
    ui->editSaveLocation->installEventFilter(this);

    setWindowTitle(tr("Program Settings"));
}

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

            ui->editSaveLocation->setText(mLocation);
        }

        ui->editSaveLocation->clearFocus();
    }

    return false;
}

void SettingsDialog::SetSaveLocation(QString location)
{
    alternateSaveLocation = location;
    ui->editSaveLocation->setText(location);
}

void SettingsDialog::SetSpreadsheetOption(bool value)
{
    spreadsheetOutput = value;
    ui->checkBoxSheets->setChecked(value);
}

void SettingsDialog::SetDisplayOption(bool value)
{
    displayPlots = value;
    ui->checkBoxGraphs->setChecked(value);
}

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

SettingsDialog::~SettingsDialog()
{
    delete ui;
}

void SettingsDialog::on_pushButton_clicked()
{
    alternateSaveLocation = ui->editSaveLocation->text();
    spreadsheetOutput = ui->checkBoxSheets->isChecked();
    displayPlots = ui->checkBoxGraphs->isChecked();
    displayDark = (ui->comboTheme->currentIndex() == 1);

    accept();
}
