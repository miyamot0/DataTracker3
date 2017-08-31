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

#include "displaytabledialog.h"
#include "ui_displaytabledialog.h"

#include "windowtools.h"

DisplayTableDialog::DisplayTableDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisplayTableDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("Results Table"));

    WindowTools::SetDialogFixedMaximize(this);
}

void DisplayTableDialog::InsertData(QList<QStringList> data)
{
    ui->tableWidget->clear();
    ui->tableWidget->setRowCount(0);
    ui->tableWidget->setColumnCount(0);

    QStringList temp;

    for (int i(0); i<data.length(); i++)
    {
        temp = data.at(i);
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());

        for (int j(0); j<temp.length(); j++)
        {

            if(j >= ui->tableWidget->columnCount())
            {
                ui->tableWidget->insertColumn(ui->tableWidget->columnCount());
            }

            // Less one to adjust for Excel-style coordinates
            ui->tableWidget->setItem(i - 1, j - 1, new QTableWidgetItem(temp.at(j)));
        }
    }
}

DisplayTableDialog::~DisplayTableDialog()
{
    delete ui;
}
