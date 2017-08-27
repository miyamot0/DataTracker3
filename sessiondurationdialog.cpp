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

#include "sessiondurationdialog.h"
#include "ui_sessiondurationdialog.h"

SessionDurationDialog::SessionDurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SessionDurationDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("Set Session Duration"));
}

/** Pull out and augment result
 * @brief SessionDurationDialog::GetSeconds
 * @return
 */
int SessionDurationDialog::GetSeconds()
{
    int seconds = ui->spinBox->value();

    if (ui->comboBox->currentIndex() == 0)
    {
        return seconds * 60 * 60;
    }
    else if (ui->comboBox->currentIndex() == 1)
    {
        return seconds * 60;
    }
    else if (ui->comboBox->currentIndex() == 2)
    {
        return seconds;
    }
    else
    {
        return seconds;
    }
}

/** Flag good to go
 * @brief SessionDurationDialog::done
 * @param r
 */
void SessionDurationDialog::done(int r)
{
    AcceptDialog = true;

    QDialog::done(r);
}

SessionDurationDialog::~SessionDurationDialog()
{
    delete ui;
}
