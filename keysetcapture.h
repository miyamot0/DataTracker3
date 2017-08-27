#ifndef KEYSETCAPTURE_H
#define KEYSETCAPTURE_H

#include <QDialog>
#include <QKeyEvent>
#include <QAbstractButton>

namespace Ui {
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

class KeySetCapture;
}

class KeySetCapture : public QDialog
{
    Q_OBJECT

public:
    explicit KeySetCapture(QWidget *parent = 0);

    QKeyEvent *mKey;

    bool enteredKey = false;

    QString KeyDescription;
    QString KeyText;
    int KeyCode;

    void SetKeyCode(int value);
    void SetKeyDescription(QString value);
    void SetKeyText(QString value);

    ~KeySetCapture();

private slots:
    void on_buttonKey_clicked();
    void on_buttonBox_clicked(QAbstractButton *button);

protected:
    bool eventFilter(QObject *, QEvent *e);

private:
    Ui::KeySetCapture *ui;

    bool waitingOnInput = false;
};

#endif // KEYSETCAPTURE_H
