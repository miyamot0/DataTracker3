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

#include "keysetcapture.h"
#include "ui_keysetcapture.h"

KeySetCapture::KeySetCapture(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeySetCapture)
{
    ui->setupUi(this);

    ui->editKey->setText("No Key Set");

    installEventFilter(this);

    setWindowTitle(tr("Capture Keys"));
}

KeySetCapture::~KeySetCapture()
{
    delete ui;
}

/**
 * @brief KeySetCapture::SetKeyCode
 * @param value
 */
void KeySetCapture::SetKeyCode(int value)
{
    KeyCode = value;
}

/**
 * @brief KeySetCapture::SetKeyDescription
 * @param value
 */
void KeySetCapture::SetKeyDescription(QString value)
{
    KeyDescription = value;

    ui->editDescription->setText(value);
}

/**
 * @brief KeySetCapture::SetKeyText
 * @param value
 */
void KeySetCapture::SetKeyText(QString value)
{
    KeyText = value;

    ui->editKey->setText(QString("Key Set: %1").arg(value.toUpper()));
}

/**
 * @brief KeySetCapture::eventFilter
 * @param e
 * @return
 */
bool KeySetCapture::eventFilter(QObject *, QEvent *e)
{
    if (e->type() == QEvent::KeyRelease && waitingOnInput)
    {
        mKey = static_cast<QKeyEvent *>(e);

        if (mKey->key() == Qt::Key_Shift ||
            mKey->key() == Qt::Key_Control ||
            mKey->key() == Qt::Key_Escape ||
            mKey->key() == Qt::Key_Z ||
            mKey->key() == Qt::Key_X ||
            mKey->key() == Qt::Key_C ||
            mKey->key() == Qt::Key_Tab)
        {
            return false;
        }

        ui->editDescription->setReadOnly(false);
        ui->editDescription->setFocus();

        KeyCode = mKey->key();
        KeyText = mKey->text();
        KeyDescription = ui->editDescription->text();

        ui->editKey->setText(QString("Key Set: %1").arg(mKey->text().toUpper()));
        waitingOnInput = false;
        ui->buttonKey->setEnabled(true);
        ui->editKey->setStyleSheet(QString("QLineEdit { background: green; padding: 10 20 }"));

        enteredKey = true;
    }

    return false;
}

/**
 * @brief KeySetCapture::on_buttonKey_clicked
 */
void KeySetCapture::on_buttonKey_clicked()
{
    if (waitingOnInput)
    {
        return;
    }

    ui->editDescription->setReadOnly(true);

    mKey = NULL;
    ui->editKey->setText("Waiting for Key Press");
    waitingOnInput = true;
    ui->buttonKey->setEnabled(false);
    ui->editKey->setStyleSheet(QString("QLineEdit { background: red; padding: 10 20 }"));

    enteredKey = false;

    KeyCode = -1;
    KeyText = "";
    KeyDescription = "";

    ui->buttonKey->setFocus();
}

/**
 * @brief KeySetCapture::on_buttonBox_clicked
 * @param button
 */
void KeySetCapture::on_buttonBox_clicked(QAbstractButton *button)
{
    if((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Ok))
    {
        KeyDescription = ui->editDescription->text();
    }
}
