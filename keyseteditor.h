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

#ifndef KEYSETEDITOR_H
#define KEYSETEDITOR_H

#include <QDialog>
#include <QAbstractItemModel>

#include "keysetcapture.h"
#include "keyset.h"

namespace Ui {
class KeySetEditor;
}

class KeySetEditor : public QDialog
{
    Q_OBJECT

public:
    explicit KeySetEditor(QWidget *parent = 0);
    ~KeySetEditor();

    KeySet keySet;
    QString KeySetName;

    void loadExistingKeys(QString path, QString group, QString individual);

private slots:
    void on_pushButtonFrequency_clicked();
    void customFrequencyMenuRequested(QPoint pos);

    void on_pushButtonDuration_clicked();
    void customDurationMenuRequested(QPoint pos);

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::KeySetEditor *ui;

    KeySetCapture *keyCapture;
};

#endif // KEYSETEDITOR_H
