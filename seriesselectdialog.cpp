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

#include "seriesselectdialog.h"
#include "ui_seriesselectdialog.h"

#include <QListWidgetItem>

SeriesSelectDialog::SeriesSelectDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SeriesSelectDialog)
{
    ui->setupUi(this);
}

/**
 * @brief SeriesSelectDialog::AddOptions
 * @param data
 */
void SeriesSelectDialog::AddOptions(QStringList data)
{
    ui->listWidget->clear();
    ui->listWidget->addItems(data);

    QListWidgetItem * mItem = 0;
    for (int i(0); i<ui->listWidget->count(); i++)
    {
        mItem = ui->listWidget->item(i);
        mItem->setFlags(mItem->flags() | Qt::ItemIsUserCheckable);
        mItem->setCheckState(Qt::Unchecked);
    }
}

/**
 * @brief SeriesSelectDialog::GetBoolList
 * @return
 */
QList<bool> SeriesSelectDialog::GetBoolList()
{
    QList<bool> returner;
    for (int i(0); i<ui->listWidget->count(); i++)
    {
        returner.append(ui->listWidget->item(i)->checkState() == Qt::Checked);

    }
    return returner;
}

SeriesSelectDialog::~SeriesSelectDialog()
{
    delete ui;
}

/**
 * @brief SeriesSelectDialog::on_pushButtonSelectAll_clicked
 */
void SeriesSelectDialog::on_pushButtonSelectAll_clicked()
{
    for (int i(0); i<ui->listWidget->count(); i++)
    {
        ui->listWidget->item(i)->setCheckState(Qt::Checked);
    }
}
