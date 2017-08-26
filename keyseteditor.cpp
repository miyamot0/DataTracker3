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

#include "keyseteditor.h"
#include "ui_keyseteditor.h"
#include "filetools.h"

#include <QMessageBox>
#include <QMenu>

KeySetEditor::KeySetEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeySetEditor)
{
    ui->setupUi(this);

    ui->tableWidgetFrequency->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidgetFrequency, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customFrequencyMenuRequested(QPoint)));

    ui->tableWidgetDuration->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->tableWidgetDuration, SIGNAL(customContextMenuRequested(QPoint)), SLOT(customDurationMenuRequested(QPoint)));
}

/**
 * @brief KeySetEditor::~KeySetEditor
 */
KeySetEditor::~KeySetEditor()
{
    delete ui;
}

/**
 * @brief KeySetEditor::on_pushButtonFrequency_clicked
 */
void KeySetEditor::on_pushButtonFrequency_clicked()
{
    keyCapture = new KeySetCapture(this);

    keyCapture->exec();

    if (keyCapture->enteredKey)
    {
        if (keyCapture->KeyCode == -1 || keyCapture->KeyDescription.length() == 0)
        {
            QMessageBox::critical(this,
              tr("DataTracker"),
              tr("Key information provided was either incomplete or missing.") );
        }
        else
        {
            ui->tableWidgetFrequency->insertRow(ui->tableWidgetFrequency->rowCount());
            ui->tableWidgetFrequency->setItem(ui->tableWidgetFrequency->rowCount() - 1, 0, new QTableWidgetItem(keyCapture->KeyText));
            ui->tableWidgetFrequency->setItem(ui->tableWidgetFrequency->rowCount() - 1, 1, new QTableWidgetItem(keyCapture->KeyDescription));
        }
    }
}

/**
 * @brief KeySetEditor::customFrequencyMenuRequested
 * @param pos
 */
void KeySetEditor::customFrequencyMenuRequested(QPoint pos)
{
    QMenu menu(this);

    QAction *editRow = menu.addAction(tr("Edit Key"));
    menu.addSeparator();
    QAction *moveRowUp = menu.addAction(tr("Move Key Up"));
    QAction *moveRowDown = menu.addAction(tr("Move Key Down"));
    menu.addSeparator();
    QAction *removeRow = menu.addAction(tr("Remove Key"));

    QAction *selectedEntry = menu.exec(ui->tableWidgetFrequency->viewport()->mapToGlobal(pos));

    QModelIndex index = ui->tableWidgetFrequency->indexAt(pos);

    if (selectedEntry == editRow)
    {
        keyCapture = new KeySetCapture(this);

        QString editingStringKey(ui->tableWidgetFrequency->item(index.row(), 0)->data(Qt::DisplayRole).toString());
        QString editingStringDesc(ui->tableWidgetFrequency->item(index.row(), 1)->data(Qt::DisplayRole).toString());

        QKeySequence seq = QKeySequence(editingStringDesc);

        keyCapture->SetKeyCode(seq[0]);
        keyCapture->SetKeyText(editingStringKey);
        keyCapture->SetKeyDescription(editingStringDesc);

        keyCapture->exec();

        if (keyCapture->enteredKey)
        {
            if (keyCapture->KeyCode == -1 || keyCapture->KeyDescription.length() == 0)
            {
                QMessageBox::critical(this,
                  tr("Error on Key Capture"),
                  tr("Key information provided was either incomplete or missing.") );
            }
            else
            {
                ui->tableWidgetFrequency->setItem(index.row(), 0, new QTableWidgetItem(keyCapture->KeyText));
                ui->tableWidgetFrequency->setItem(index.row(), 1, new QTableWidgetItem(keyCapture->KeyDescription));
            }
        }
    }
    else if (selectedEntry == removeRow)
    {
        ui->tableWidgetFrequency->removeRow(index.row());
    }
    else if (selectedEntry == moveRowUp && index.row() != 0)
    {
        QString movingStringKey(ui->tableWidgetFrequency->item(index.row(), 0)->data(Qt::DisplayRole).toString());
        QString movingStringDesc(ui->tableWidgetFrequency->item(index.row(), 1)->data(Qt::DisplayRole).toString());

        int currIndex = index.row();

        ui->tableWidgetFrequency->insertRow(currIndex - 1);

        ui->tableWidgetFrequency->setItem(currIndex - 1, 0, new QTableWidgetItem(movingStringKey));
        ui->tableWidgetFrequency->setItem(currIndex - 1, 1, new QTableWidgetItem(movingStringDesc));

        ui->tableWidgetFrequency->removeRow(currIndex + 1);
    }
    else if (selectedEntry == moveRowDown && index.row() != ui->tableWidgetFrequency->rowCount() - 1)
    {
        QString movingStringKey(ui->tableWidgetFrequency->item(index.row(), 0)->data(Qt::DisplayRole).toString());
        QString movingStringDesc(ui->tableWidgetFrequency->item(index.row(), 1)->data(Qt::DisplayRole).toString());

        int currIndex = index.row();

        ui->tableWidgetFrequency->insertRow(currIndex + 2);

        ui->tableWidgetFrequency->setItem(currIndex + 2, 0, new QTableWidgetItem(movingStringKey));
        ui->tableWidgetFrequency->setItem(currIndex + 2, 1, new QTableWidgetItem(movingStringDesc));

        ui->tableWidgetFrequency->removeRow(currIndex);
    }
}

/**
 * @brief KeySetEditor::on_pushButtonDuration_clicked
 */
void KeySetEditor::on_pushButtonDuration_clicked()
{
    keyCapture = new KeySetCapture(this);

    keyCapture->exec();

    if (keyCapture->enteredKey)
    {
        if (keyCapture->KeyCode == -1 || keyCapture->KeyDescription.length() == 0)
        {
            QMessageBox::critical(this,
              tr("DataTracker"),
              tr("Key information provided was either incomplete or missing.") );
        }
        else
        {
            ui->tableWidgetDuration->insertRow(ui->tableWidgetDuration->rowCount());
            ui->tableWidgetDuration->setItem(ui->tableWidgetDuration->rowCount() - 1, 0, new QTableWidgetItem(keyCapture->KeyText));
            ui->tableWidgetDuration->setItem(ui->tableWidgetDuration->rowCount() - 1, 1, new QTableWidgetItem(keyCapture->KeyDescription));
        }
    }
}

/**
 * @brief KeySetEditor::customDurationMenuRequested
 * @param pos
 */
void KeySetEditor::customDurationMenuRequested(QPoint pos)
{
    QMenu menu(this);

    QAction *editRow = menu.addAction(tr("Edit Key"));
    menu.addSeparator();
    QAction *moveRowUp = menu.addAction(tr("Move Key Up"));
    QAction *moveRowDown = menu.addAction(tr("Move Key Down"));
    menu.addSeparator();
    QAction *removeRow = menu.addAction(tr("Remove Key"));

    QAction *selectedEntry = menu.exec(ui->tableWidgetDuration->viewport()->mapToGlobal(pos));

    QModelIndex index = ui->tableWidgetDuration->indexAt(pos);

    if (selectedEntry == editRow)
    {
        keyCapture = new KeySetCapture(this);

        QString editingStringKey(ui->tableWidgetDuration->item(index.row(), 0)->data(Qt::DisplayRole).toString());
        QString editingStringDesc(ui->tableWidgetDuration->item(index.row(), 1)->data(Qt::DisplayRole).toString());

        QKeySequence seq = QKeySequence(editingStringDesc);

        keyCapture->SetKeyCode(seq[0]);
        keyCapture->SetKeyText(editingStringKey);
        keyCapture->SetKeyDescription(editingStringDesc);

        keyCapture->exec();

        if (keyCapture->enteredKey)
        {
            if (keyCapture->KeyCode == -1 || keyCapture->KeyDescription.length() == 0)
            {
                QMessageBox::critical(this,
                  tr("Error on Key Capture"),
                  tr("Key information provided was either incomplete or missing.") );
            }
            else
            {
                ui->tableWidgetDuration->setItem(index.row(), 0, new QTableWidgetItem(keyCapture->KeyText));
                ui->tableWidgetDuration->setItem(index.row(), 1, new QTableWidgetItem(keyCapture->KeyDescription));
            }
        }
    }
    else if (selectedEntry == removeRow)
    {
        ui->tableWidgetDuration->removeRow(index.row());
    }
    else if (selectedEntry == moveRowUp && index.row() != 0)
    {
        QString movingStringKey(ui->tableWidgetDuration->item(index.row(), 0)->data(Qt::DisplayRole).toString());
        QString movingStringDesc(ui->tableWidgetDuration->item(index.row(), 1)->data(Qt::DisplayRole).toString());

        int currIndex = index.row();

        ui->tableWidgetDuration->insertRow(currIndex - 1);

        ui->tableWidgetDuration->setItem(currIndex - 1, 0, new QTableWidgetItem(movingStringKey));
        ui->tableWidgetDuration->setItem(currIndex - 1, 1, new QTableWidgetItem(movingStringDesc));

        ui->tableWidgetDuration->removeRow(currIndex + 1);
    }
    else if (selectedEntry == moveRowDown && index.row() != ui->tableWidgetDuration->rowCount() - 1)
    {
        QString movingStringKey(ui->tableWidgetDuration->item(index.row(), 0)->data(Qt::DisplayRole).toString());
        QString movingStringDesc(ui->tableWidgetDuration->item(index.row(), 1)->data(Qt::DisplayRole).toString());

        int currIndex = index.row();

        ui->tableWidgetDuration->insertRow(currIndex + 2);

        ui->tableWidgetDuration->setItem(currIndex + 2, 0, new QTableWidgetItem(movingStringKey));
        ui->tableWidgetDuration->setItem(currIndex + 2, 1, new QTableWidgetItem(movingStringDesc));

        ui->tableWidgetDuration->removeRow(currIndex);
    }
}

/**
 * @brief KeySetEditor::loadExistingKeys
 * @param path
 * @param group
 * @param individual
 */
void KeySetEditor::loadExistingKeys(QString path, QString group, QString individual)
{
    QString mKeyPath = FileTools::pathAppend(path, group);
    mKeyPath = FileTools::pathAppend(mKeyPath, individual);

    FileTools::ReadKeySet(FileTools::pathAppend(mKeyPath, QString("%1.json").arg(KeySetName)), &keySet);

    ui->tableWidgetFrequency->setRowCount(0);

    for (KeySetEntry mKey : keySet.FrequencyKeys)
    {
        ui->tableWidgetFrequency->insertRow(ui->tableWidgetFrequency->rowCount());
        ui->tableWidgetFrequency->setItem(ui->tableWidgetFrequency->rowCount() - 1, 0, new QTableWidgetItem(mKey.KeyName));
        ui->tableWidgetFrequency->setItem(ui->tableWidgetFrequency->rowCount() - 1, 1, new QTableWidgetItem(mKey.KeyDescription));
    }

    ui->tableWidgetDuration->setRowCount(0);

    for (KeySetEntry mKey : keySet.DurationKeys)
    {
        ui->tableWidgetDuration->insertRow(ui->tableWidgetDuration->rowCount());
        ui->tableWidgetDuration->setItem(ui->tableWidgetDuration->rowCount() - 1, 0, new QTableWidgetItem(mKey.KeyName));
        ui->tableWidgetDuration->setItem(ui->tableWidgetDuration->rowCount() - 1, 1, new QTableWidgetItem(mKey.KeyDescription));
    }
}

/**
 * @brief KeySetEditor::on_buttonBox_clicked
 * @param button
 */
void KeySetEditor::on_buttonBox_clicked(QAbstractButton *button)
{
    if((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Ok))
    {
        // TODO keySet.KeySetName

        keySet.FrequencyKeys.clear();

        QKeySequence seq;
        QString nameKey, descKey;

        for (int i=0; i<ui->tableWidgetFrequency->rowCount(); i++)
        {
            keySet.FrequencyKeys.append(KeySetEntry());

            nameKey = ui->tableWidgetFrequency->item(i, 0)->data(Qt::DisplayRole).toString();
            keySet.FrequencyKeys[i].KeyName = nameKey;

            seq = QKeySequence(nameKey);
            keySet.FrequencyKeys[i].KeyCode = seq[0];

            descKey = ui->tableWidgetFrequency->item(i, 1)->data(Qt::DisplayRole).toString();
            keySet.FrequencyKeys[i].KeyDescription = descKey;
        }

        keySet.DurationKeys.clear();

        for (int i=0; i<ui->tableWidgetDuration->rowCount(); i++)
        {
            keySet.DurationKeys.append(KeySetEntry());

            nameKey = ui->tableWidgetDuration->item(i, 0)->data(Qt::DisplayRole).toString();
            keySet.DurationKeys[i].KeyName = nameKey;

            seq = QKeySequence(nameKey);
            keySet.DurationKeys[i].KeyCode = seq[0];

            descKey = ui->tableWidgetDuration->item(i, 1)->data(Qt::DisplayRole).toString();
            keySet.DurationKeys[i].KeyDescription = descKey;
        }
    }
}
