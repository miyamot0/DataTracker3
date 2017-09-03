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

#include <QDir>
#include <QtXlsx>
#include <QFileDialog>

DisplayTableDialog::DisplayTableDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisplayTableDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("Results Table"));

    saveAction = new QAction(tr("&Save"));
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save results to file"));

    closeAction = new QAction(tr("&Close"));
    closeAction->setShortcuts(QKeySequence::Close);

    connect(saveAction, &QAction::triggered, this, &DisplayTableDialog::SaveToFile);
    connect(closeAction, &QAction::triggered, this, &DisplayTableDialog::close);

    menuBar = new QMenuBar(0);

    fileMenu = menuBar->addMenu(tr("&File"));
    fileMenu->addAction(saveAction);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAction);

    layout()->setMenuBar(menuBar);

    WindowTools::SetDialogFixedMaximize(this);
}

/**
 * @brief DisplayTableDialog::SaveToFile
 */
void DisplayTableDialog::SaveToFile()
{
    QString file_name;
    QString fileFilter = "Spreadsheet (*.xlsx)";

#ifdef _WIN32

        file_name = QFileDialog::getSaveFileName(this, tr("Save spreadsheet file"), QDir::homePath(),
                                         fileFilter);

#elif TARGET_OS_MAC

        file_name = QFileDialog::getSaveFileName(this, tr("Save spreadsheet file"), QDir::homePath(),
                                         fileFilter, &fileFilter, QFileDialog::Option::DontUseNativeDialog);

        if (!file_name.contains(".xlsx"))
        {
            file_name.append(".xlsx");
        }

#endif

    if(!file_name.trimmed().isEmpty())
    {
        QApplication::setOverrideCursor(Qt::WaitCursor);

        QXlsx::Document xlsx;

        int rows = ui->tableWidget->rowCount();
        int cols = ui->tableWidget->columnCount();

        QString temp;

        for (int i=0; i<rows; i++)
        {
            for (int j=0; j<cols; j++)
            {
                QTableWidgetItem *item = ui->tableWidget->item(i, j);

                if (item != NULL && !item->text().isEmpty())
                {
                    temp = ui->tableWidget->item(i, j)->data(Qt::DisplayRole).toString();
                    xlsx.write(i + 1, j + 1, temp);
                }
            }
        }

        xlsx.saveAs(file_name);

        QApplication::restoreOverrideCursor();
    }
}

/**
 * @brief DisplayTableDialog::InsertData
 * @param data
 */
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
