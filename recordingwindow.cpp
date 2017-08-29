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

#include "recordingwindow.h"
#include "ui_recordingwindow.h"
#include "sessionevent.h"

#include <QKeyEvent>
#include <QMessageBox>
#include <QDebug>

RecordingWindow::RecordingWindow(QWidget *parent) : QDialog(parent), ui(new Ui::RecordingWindow)
{
    ui->setupUi(this);

    installEventFilter(this);

    setWindowTitle(tr("Session Recording Window"));

    Started = false;
}

/**
 * @brief RecordingWindow::init
 */
void RecordingWindow::init()
{
    baseTimer.stop();
    DurationFlags.clear();
    DurationSums.clear();
    DurationFlaggedTimes.clear();

    ScheduleFlags.clear();
    ScheduleDurationSums.clear();
    ScheduleDurationFlaggedTimes.clear();

    PressedKeys.clear();

    while (ui->tableWidgetLog->rowCount() > 0)
    {
        ui->tableWidgetLog->removeRow(0);
    }

    ui->editTimerBase->setText("00:00:00:000");
    ui->editTimerOne->setText("00:00:00:000");
    ui->editTimerTwo->setText("00:00:00:000");
    ui->editTimerThree->setText("00:00:00:000");

    ui->editTimerBase->setStyleSheet("");
    ui->editTimerOne->setStyleSheet("");
    ui->editTimerTwo->setStyleSheet("");
    ui->editTimerThree->setStyleSheet("");

    Started = false;
    KeepData = true;
}

/**
 * @brief RecordingWindow::LoadKeys
 * @param mKeyset
 */
void RecordingWindow::LoadKeys(KeySet mKeyset)
{
    keySet = mKeyset;

    ui->tableWidgetFrequency->setRowCount(0);

    for (KeySetEntry mKey : keySet.FrequencyKeys)
    {
        ui->tableWidgetFrequency->insertRow(ui->tableWidgetFrequency->rowCount());
        ui->tableWidgetFrequency->setItem(ui->tableWidgetFrequency->rowCount() - 1, 0, new QTableWidgetItem(mKey.KeyName));
        ui->tableWidgetFrequency->setItem(ui->tableWidgetFrequency->rowCount() - 1, 1, new QTableWidgetItem(mKey.KeyDescription));
        ui->tableWidgetFrequency->setItem(ui->tableWidgetFrequency->rowCount() - 1, 2, new QTableWidgetItem("0"));
    }

    ui->tableWidgetDuration->setRowCount(0);

    for (KeySetEntry mKey : keySet.DurationKeys)
    {
        ui->tableWidgetDuration->insertRow(ui->tableWidgetDuration->rowCount());
        ui->tableWidgetDuration->setItem(ui->tableWidgetDuration->rowCount() - 1, 0, new QTableWidgetItem(mKey.KeyName));
        ui->tableWidgetDuration->setItem(ui->tableWidgetDuration->rowCount() - 1, 1, new QTableWidgetItem(mKey.KeyDescription));
        ui->tableWidgetDuration->setItem(ui->tableWidgetDuration->rowCount() - 1, 2, new QTableWidgetItem("0"));
        ui->tableWidgetDuration->setItem(ui->tableWidgetDuration->rowCount() - 1, 3, new QTableWidgetItem("0"));

        DurationFlags.append(false);
        DurationSums.append(0);
        DurationFlaggedTimes.append(QDateTime());
    }

    ui->labelTitle->setText(QString("<html><head/><body><p align='center'><span style=' font-size:12pt;'>Recording: Session #%1</span></p></body></html>").arg(keySet.Session));
}

/**
 * @brief RecordingWindow::SetGroup
 * @param value
 */
void RecordingWindow::SetGroup(QString value)
{
    ui->editGroup->setText(value);
}

/**
 * @brief RecordingWindow::SetIndividual
 * @param value
 */
void RecordingWindow::SetIndividual(QString value)
{
    ui->editPatient->setText(value);
}

/**
 * @brief RecordingWindow::SetEvaluation
 * @param value
 */
void RecordingWindow::SetEvaluation(QString value)
{
    ui->editEvaluation->setText(value);
}

/**
 * @brief RecordingWindow::SetCondition
 * @param value
 */
void RecordingWindow::SetCondition(QString value)
{
    ui->editCondition->setText(value);
}

/**
 * @brief RecordingWindow::SetCollector
 * @param value
 */
void RecordingWindow::SetCollector(QString value)
{
    ui->editCollector->setText(value);
}

/**
 * @brief RecordingWindow::SetRole
 * @param value
 */
void RecordingWindow::SetRole(QString value)
{
    ui->editPrimary->setText(value);
}

/**
 * @brief RecordingWindow::UpdateGUI
 */
void RecordingWindow::UpdateGUI()
{
    endTime = QDateTime::currentDateTime();

    ui->editTimerBase->setText(formatTimeLabel(startTime.msecsTo(endTime)));

    ParseTimes();

    if ((startTime.msecsTo(endTime) / 1000) >= keySet.TotalSeconds)
    {
        SessionEvent loggedCloseKey;
        loggedCloseKey.TimePressed = endTime;
        loggedCloseKey.MeasurementType = Measurement::Schedule;
        loggedCloseKey.ScheduleType = CurrentSchedule;

        KeySetEntry loggedClosedKeySet;

        if (CurrentSchedule == Schedule::One)
        {
            loggedClosedKeySet.KeyCode = Qt::Key_Z;
            loggedClosedKeySet.KeyName = "Schedule 1 End";
            loggedClosedKeySet.KeyDescription = "Schedule 1 End";
        }
        else if (CurrentSchedule == Schedule::Two)
        {
            loggedClosedKeySet.KeyCode = Qt::Key_X;
            loggedClosedKeySet.KeyName = "Schedule 2 End";
            loggedClosedKeySet.KeyDescription = "Schedule 2 End";
        }
        else if (CurrentSchedule == Schedule::Three)
        {
            loggedClosedKeySet.KeyCode = Qt::Key_C;
            loggedClosedKeySet.KeyName = "Schedule 3 End";
            loggedClosedKeySet.KeyDescription = "Schedule 3 End";
        }

        loggedCloseKey.KeyEntered = loggedClosedKeySet;

        AddKey(loggedCloseKey);

        accept();
    }
}

/**
 * @brief RecordingWindow::reject
 */
void RecordingWindow::reject()
{
    if (!Started)
    {
        KeepData = false;

        baseTimer.stop();

        QDialog::reject();
        QDialog::close();
    }
    else
    {
        QMessageBox::StandardButton resBtn = QMessageBox::question( this, tr("Session Recording"),
                                                                    tr("Are you sure you want to end session?"),
                                                                    QMessageBox::Cancel |
                                                                    QMessageBox::No |
                                                                    QMessageBox::Yes,
                                                                    QMessageBox::Yes);
        if (resBtn == QMessageBox::Yes)
        {
            baseTimer.stop();

            endTime = QDateTime::currentDateTime();

            ui->editTimerBase->setText(formatTimeLabel(startTime.msecsTo(endTime)));

            ParseTimes();

            SessionEvent loggedCloseKey;
            loggedCloseKey.TimePressed = endTime;
            loggedCloseKey.MeasurementType = Measurement::Schedule;
            loggedCloseKey.ScheduleType = CurrentSchedule;

            KeySetEntry loggedClosedKeySet;

            if (CurrentSchedule == Schedule::One)
            {
                loggedClosedKeySet.KeyCode = Qt::Key_Z;
                loggedClosedKeySet.KeyName = "Schedule 1 End";
                loggedClosedKeySet.KeyDescription = "Schedule 1 End";
            }
            else if (CurrentSchedule == Schedule::Two)
            {
                loggedClosedKeySet.KeyCode = Qt::Key_X;
                loggedClosedKeySet.KeyName = "Schedule 2 End";
                loggedClosedKeySet.KeyDescription = "Schedule 2 End";
            }
            else if (CurrentSchedule == Schedule::Three)
            {
                loggedClosedKeySet.KeyCode = Qt::Key_C;
                loggedClosedKeySet.KeyName = "Schedule 3 End";
                loggedClosedKeySet.KeyDescription = "Schedule 3 End";
            }

            loggedCloseKey.KeyEntered = loggedClosedKeySet;

            AddKey(loggedCloseKey);

            QMessageBox::StandardButton saveData = QMessageBox::question(this,
                                                                         tr("Keep Recorded Data?"),
                                                                         tr("Do you want to discard this data?"),
                                                                         QMessageBox::No |
                                                                         QMessageBox::Yes);


            if (saveData == QMessageBox::No)
            {
                QDialog::accept();

                KeepData = true;
            }
            else
            {
                QDialog::reject();

                KeepData = false;
            }
        }
    }
}

/**
 * @brief RecordingWindow::StartTiming
 */
void RecordingWindow::StartTiming()
{
    Started = true;

    ScheduleFlags.append(true);
    ScheduleFlags.append(false);
    ScheduleFlags.append(false);

    ScheduleDurationSums.append(0);
    ScheduleDurationSums.append(0);
    ScheduleDurationSums.append(0);

    ScheduleDurationFlaggedTimes.append(QDateTime());
    ScheduleDurationFlaggedTimes.append(QDateTime());
    ScheduleDurationFlaggedTimes.append(QDateTime());

    connect(&baseTimer, SIGNAL(timeout()), this, SLOT(UpdateGUI()));
    baseTimer.start(50);

    startTime = QDateTime::currentDateTime();

    SessionEvent loggedKey;
    loggedKey.TimePressed = startTime;
    loggedKey.MeasurementType = Measurement::Schedule;
    loggedKey.ScheduleType = Schedule::One;

    KeySetEntry loggedKeySet;
    loggedKeySet.KeyCode = Qt::Key_Z;
    loggedKeySet.KeyName = "Schedule One Start";
    loggedKeySet.KeyDescription = "Schedule One Start";

    loggedKey.KeyEntered = loggedKeySet;

    AddKey(loggedKey);

    CurrentSchedule = Schedule::One;

    ui->editTimerOne->setStyleSheet("background: green;");
}

/**
 * @brief RecordingWindow::eventFilter
 * @param e
 * @return
 */
bool RecordingWindow::eventFilter(QObject *, QEvent *e)
{
    if (e->type() == QEvent::KeyRelease && !Started)
    {
        QKeyEvent * mKey = static_cast<QKeyEvent *>(e);

        if (mKey->key() == Qt::Key_Tab)
        {
            StartTiming();
        }
    }
    else if (e->type() == QEvent::KeyRelease && Started)
    {
        QKeyEvent * mKey = static_cast<QKeyEvent *>(e);

        DetectFrequencyKey(mKey);
        DetectDurationKey(mKey);
        DetectScheduleKey(mKey);
        DetectEndKey(mKey);
    }

    return false;
}

/**
 * @brief RecordingWindow::DetectScheduleKey
 * @param mKey
 */
void RecordingWindow::DetectScheduleKey(QKeyEvent * mKey)
{
    if (mKey->key() == Qt::Key_Z)
    {
        if (CurrentSchedule == Schedule::One)
        {
            return;
        }
        else
        {
            Schedule scheduleTrans = (CurrentSchedule == Schedule::Two) ? Schedule::Two : Schedule::Three;
            int scheduleTransCode = (CurrentSchedule == Schedule::Two) ? Qt::Key_X : Qt::Key_C;
            QString scheduleTransString = (CurrentSchedule == Schedule::Two) ? "Schedule Two End" : "Schedule Three End";

            SessionEvent endOldSchedule;
            endOldSchedule.TimePressed = QDateTime::currentDateTime();
            endOldSchedule.MeasurementType = Measurement::Schedule;
            endOldSchedule.ScheduleType = scheduleTrans;

            KeySetEntry loggedScheduleSet;
            loggedScheduleSet.KeyCode = scheduleTransCode;
            loggedScheduleSet.KeyName = scheduleTransString;
            loggedScheduleSet.KeyDescription = scheduleTransString;

            endOldSchedule.KeyEntered = loggedScheduleSet;

            AddKey(endOldSchedule);

            ui->editTimerTwo->setStyleSheet("");
            ui->editTimerThree->setStyleSheet("");

            UpdateTables();
        }

        SessionEvent loggedKey;
        loggedKey.TimePressed = QDateTime::currentDateTime();
        loggedKey.MeasurementType = Measurement::Schedule;
        loggedKey.ScheduleType = Schedule::One;

        KeySetEntry loggedKeySet;
        loggedKeySet.KeyCode = mKey->key();
        loggedKeySet.KeyName = "Schedule One Start";
        loggedKeySet.KeyDescription = "Schedule One Start";

        loggedKey.KeyEntered = loggedKeySet;

        AddKey(loggedKey);

        ScheduleFlags[0] = true;
        ScheduleFlags[1] = false;
        ScheduleFlags[2] = false;

        CurrentSchedule = loggedKey.ScheduleType;

        ui->editTimerOne->setStyleSheet("background: green;");

        UpdateTables();
    }

    if (mKey->key() == Qt::Key_X)
    {
        if (CurrentSchedule == Schedule::Two)
        {
            return;
        }
        else
        {
            Schedule scheduleTrans = (CurrentSchedule == Schedule::One) ? Schedule::One : Schedule::Three;
            int scheduleTransCode = (CurrentSchedule == Schedule::One) ? Qt::Key_Z : Qt::Key_C;
            QString scheduleTransString = (CurrentSchedule == Schedule::One) ? "Schedule One End" : "Schedule Three End";

            SessionEvent endOldSchedule;
            endOldSchedule.TimePressed = QDateTime::currentDateTime();
            endOldSchedule.MeasurementType = Measurement::Schedule;
            endOldSchedule.ScheduleType = scheduleTrans;

            KeySetEntry loggedScheduleSet;
            loggedScheduleSet.KeyCode = scheduleTransCode;
            loggedScheduleSet.KeyName = scheduleTransString;
            loggedScheduleSet.KeyDescription = scheduleTransString;

            endOldSchedule.KeyEntered = loggedScheduleSet;

            AddKey(endOldSchedule);

            ui->editTimerOne->setStyleSheet("");
            ui->editTimerThree->setStyleSheet("");

            UpdateTables();
        }

        SessionEvent loggedKey;
        loggedKey.TimePressed = QDateTime::currentDateTime();
        loggedKey.MeasurementType = Measurement::Schedule;
        loggedKey.ScheduleType = Schedule::Two;

        KeySetEntry loggedKeySet;
        loggedKeySet.KeyCode = mKey->key();
        loggedKeySet.KeyName = "Schedule Two Start";
        loggedKeySet.KeyDescription = "Schedule Two Start";

        loggedKey.KeyEntered = loggedKeySet;

        AddKey(loggedKey);

        ScheduleFlags[0] = false;
        ScheduleFlags[1] = true;
        ScheduleFlags[2] = false;

        CurrentSchedule = loggedKey.ScheduleType;

        ui->editTimerTwo->setStyleSheet("background: yellow;");

        UpdateTables();
    }

    if (mKey->key() == Qt::Key_C)
    {
        if (CurrentSchedule == Schedule::Three)
        {
            return;
        }
        else
        {
            Schedule scheduleTrans = (CurrentSchedule == Schedule::One) ? Schedule::One : Schedule::Two;
            int scheduleTransCode = (CurrentSchedule == Schedule::One) ? Qt::Key_Z : Qt::Key_X;
            QString scheduleTransString = (CurrentSchedule == Schedule::One) ? "Schedule One" : "Schedule Two";

            SessionEvent endOldSchedule;
            endOldSchedule.TimePressed = QDateTime::currentDateTime();
            endOldSchedule.MeasurementType = Measurement::Schedule;
            endOldSchedule.ScheduleType = scheduleTrans;

            KeySetEntry loggedScheduleSet;
            loggedScheduleSet.KeyCode = scheduleTransCode;
            loggedScheduleSet.KeyName = scheduleTransString;
            loggedScheduleSet.KeyDescription = scheduleTransString;

            endOldSchedule.KeyEntered = loggedScheduleSet;

            AddKey(endOldSchedule);

            ui->editTimerOne->setStyleSheet("");
            ui->editTimerTwo->setStyleSheet("");

            UpdateTables();
        }

        SessionEvent loggedKey;
        loggedKey.TimePressed = QDateTime::currentDateTime();
        loggedKey.MeasurementType = Measurement::Schedule;
        loggedKey.ScheduleType = Schedule::Three;

        KeySetEntry loggedKeySet;
        loggedKeySet.KeyCode = mKey->key();
        loggedKeySet.KeyName = "Schedule Three Start";
        loggedKeySet.KeyDescription = "Schedule Three Start";

        loggedKey.KeyEntered = loggedKeySet;

        AddKey(loggedKey);

        ScheduleFlags[0] = false;
        ScheduleFlags[1] = false;
        ScheduleFlags[2] = true;

        CurrentSchedule = loggedKey.ScheduleType;

        ui->editTimerThree->setStyleSheet("background: red;");

        UpdateTables();
    }
}

/**
 * @brief RecordingWindow::DetectFrequencyKey
 * @param mKey
 */
void RecordingWindow::DetectFrequencyKey(QKeyEvent * mKey)
{
    for (int i=0; i<keySet.FrequencyKeys.count(); i++)
    {
        if (mKey->key() == keySet.FrequencyKeys.at(i).KeyCode)
        {
            SessionEvent loggedKey;
            loggedKey.TimePressed = QDateTime::currentDateTime();
            loggedKey.MeasurementType = Measurement::Rate;
            loggedKey.ScheduleType = CurrentSchedule;

            KeySetEntry loggedKeySet;
            loggedKeySet.KeyCode = mKey->key();
            loggedKeySet.KeyName = mKey->text();
            loggedKeySet.KeyDescription = mKey->text();

            loggedKey.KeyEntered = loggedKeySet;

            AddKey(loggedKey);
        }
    }
}

/**
 * @brief RecordingWindow::DetectDurationKey
 * @param mKey
 */
void RecordingWindow::DetectDurationKey(QKeyEvent * mKey)
{
    for (int i=0; i<keySet.DurationKeys.count(); i++)
    {
        if (mKey->key() == keySet.DurationKeys.at(i).KeyCode)
        {
            SessionEvent loggedKey;
            loggedKey.TimePressed = QDateTime::currentDateTime();
            loggedKey.MeasurementType = Measurement::Time;
            loggedKey.ScheduleType = CurrentSchedule;

            KeySetEntry loggedKeySet;
            loggedKeySet.KeyCode = mKey->key();
            loggedKeySet.KeyName = mKey->text();
            loggedKeySet.KeyDescription = mKey->text();

            loggedKey.KeyEntered = loggedKeySet;

            AddKey(loggedKey);
        }
    }
}

/**
 * @brief RecordingWindow::DetectEndKey
 * @param mKey
 */
void RecordingWindow::DetectEndKey(QKeyEvent * mKey)
{
    if (mKey->key() == Qt::Key_Backspace)
    {
        RemoveKey();
    }
}

/**
 * @brief RecordingWindow::AddKey
 * @param pressedKey
 */
void RecordingWindow::AddKey(SessionEvent pressedKey)
{
    PressedKeys.append(pressedKey);

    ui->tableWidgetLog->insertRow(ui->tableWidgetLog->rowCount());

    ui->tableWidgetLog->setItem(ui->tableWidgetLog->rowCount() - 1, 0,
                                new QTableWidgetItem(pressedKey.KeyEntered.KeyName));
    ui->tableWidgetLog->setItem(ui->tableWidgetLog->rowCount() - 1, 1,
                                new QTableWidgetItem(formatScheduleString(CurrentSchedule)));
    ui->tableWidgetLog->setItem(ui->tableWidgetLog->rowCount() - 1, 2,
                                new QTableWidgetItem(formatTimeLabel(startTime.msecsTo(pressedKey.TimePressed))));

    ui->tableWidgetLog->scrollToBottom();

    UpdateTables();
}

/**
 * @brief RecordingWindow::RemoveKey
 */
void RecordingWindow::RemoveKey()
{
    if (PressedKeys.count() == 0)
    {
        return;
    }

    PressedKeys.removeLast();

    ui->tableWidgetLog->removeRow(ui->tableWidgetLog->rowCount() - 1);

    UpdateTables();
}

/**
 * @brief RecordingWindow::UpdateTables
 */
void RecordingWindow::UpdateTables()
{
    for (int i=0; i<keySet.FrequencyKeys.count(); i++)
    {
        int counter = 0;

        for (int j=0; j<PressedKeys.count(); j++)
        {
            if (PressedKeys[j].MeasurementType == Measurement::Rate &&
                    PressedKeys[j].KeyEntered.KeyCode == keySet.FrequencyKeys[i].KeyCode)
            {
                counter++;
            }
        }

        ui->tableWidgetFrequency->item(i, 2)->setText(QString::number(counter));
    }

    bool waitingForNext;
    QDateTime prev, after;

    for (int i=0; i<keySet.DurationKeys.count(); i++)
    {
        int counter = 0;
        DurationSums[i] = 0;
        waitingForNext = false;

        for (int j=0; j<PressedKeys.count(); j++)
        {
            if (PressedKeys[j].MeasurementType == Measurement::Time &&
                    PressedKeys[j].KeyEntered.KeyCode == keySet.DurationKeys[i].KeyCode)
            {
                if (waitingForNext)
                {
                    after = PressedKeys[j].TimePressed;

                    waitingForNext = false;

                    DurationSums[i] = DurationSums[i] + prev.msecsTo(after);
                }
                else
                {
                    prev = PressedKeys[j].TimePressed;

                    DurationFlaggedTimes[i] = prev;

                    waitingForNext = true;
                }

                counter++;
            }
        }

        ui->tableWidgetDuration->item(i, 2)->setText(QString::number((int)(((double)counter / 2)+(0.5))));

        DurationFlags[i] = (counter % 2) == 1;
    }

    int keyCode;

    for (int i=0; i<ScheduleDurationSums.count(); i++)
    {
        int counter = 0;
        ScheduleDurationSums[i] = 0;
        waitingForNext = false;

        if (i == 0)
        {
            keyCode = Qt::Key_Z;
        }
        else if (i == 1)
        {
            keyCode = Qt::Key_X;
        }
        else if (i == 2)
        {
            keyCode = Qt::Key_C;
        }

        for (int j=0; j<PressedKeys.count(); j++)
        {
            if (PressedKeys[j].MeasurementType == Measurement::Schedule && PressedKeys[j].KeyEntered.KeyCode == keyCode)
            {
                if (waitingForNext)
                {
                    after = PressedKeys[j].TimePressed;

                    waitingForNext = false;

                    ScheduleDurationSums[i] = ScheduleDurationSums[i] + prev.msecsTo(after);
                }
                else
                {
                    prev = PressedKeys[j].TimePressed;

                    ScheduleDurationFlaggedTimes[i] = prev;

                    waitingForNext = true;
                }

                counter++;
            }
        }

        ScheduleFlags[i] = (counter % 2) == 1;
    }

}

/**
 * @brief RecordingWindow::ParseTimes
 */
void RecordingWindow::ParseTimes()
{
    for (int i=0; i<DurationSums.length(); i++)
    {
        if (DurationFlags[i])
        {
            qint64 extraTime = DurationFlaggedTimes[i].msecsTo(QDateTime::currentDateTime());
            ui->tableWidgetDuration->item(i, 3)->setText(formatTimeLabel(DurationSums[i] + extraTime));
        }
        else
        {
            ui->tableWidgetDuration->item(i, 3)->setText(formatTimeLabel(DurationSums[i]));
        }
    }

    QLineEdit * mEditRef;

    for (int i=0; i<ScheduleDurationSums.count(); i++)
    {
        if (i == 0)
        {
            mEditRef = ui->editTimerOne;
        }
        else if (i == 1)
        {
            mEditRef = ui->editTimerTwo;
        }
        else if (i == 2)
        {
            mEditRef = ui->editTimerThree;
        }

        if (ScheduleFlags[i])
        {
            qint64 extraTime = ScheduleDurationFlaggedTimes[i].msecsTo(QDateTime::currentDateTime());
            mEditRef->setText(formatTimeLabel(ScheduleDurationSums[i] + extraTime));
        }
        else
        {
            mEditRef->setText(formatTimeLabel(ScheduleDurationSums[i]));
        }
    }
}

/**
 * @brief RecordingWindow::~RecordingWindow
 */
RecordingWindow::~RecordingWindow()
{
    delete ui;
}
