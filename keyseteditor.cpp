#include "keyseteditor.h"
#include "ui_keyseteditor.h"

#include <QDebug>
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

KeySetEditor::~KeySetEditor()
{
    delete ui;
}

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

void KeySetEditor::customFrequencyMenuRequested(QPoint pos)
{
    QMenu menu(this);

    QAction *removeRow = menu.addAction(tr("Remove"));

    menu.addSeparator();

    QAction *moveRowUp = menu.addAction(tr("Move Key Up"));

    QAction *moveRowDown = menu.addAction(tr("Move Key Down"));

    QAction *selectedEntry = menu.exec(ui->tableWidgetFrequency->viewport()->mapToGlobal(pos));

    QModelIndex index = ui->tableWidgetFrequency->indexAt(pos);

    if (selectedEntry == removeRow)
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

void KeySetEditor::customDurationMenuRequested(QPoint pos)
{
    QMenu menu(this);

    QAction *removeRow = menu.addAction(tr("Remove"));

    menu.addSeparator();

    QAction *moveRowUp = menu.addAction(tr("Move Key Up"));

    QAction *moveRowDown = menu.addAction(tr("Move Key Down"));

    QAction *selectedEntry = menu.exec(ui->tableWidgetDuration->viewport()->mapToGlobal(pos));

    QModelIndex index = ui->tableWidgetDuration->indexAt(pos);

    if (selectedEntry == removeRow)
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
