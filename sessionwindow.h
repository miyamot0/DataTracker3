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

#ifndef SESSIONWINDOW_H
#define SESSIONWINDOW_H

#include <QDialog>
#include <QComboBox>
#include <QAbstractButton>

#include "recordingwindow.h"
#include "resultsdialog.h"
#include "sessioncounter.h"
#include "directoryparse.h"
#include "directorysearcher.h"
#include "keyset.h"

namespace Ui {
class SessionWindow;
}

class SessionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SessionWindow(QString mCurrentWorkingDirectory, QWidget *parent = 0);
    ~SessionWindow();

private slots:
    void on_buttonGroup_clicked();
    void on_buttonIndividual_clicked();
    void on_buttonEvaluation_clicked();
    void on_buttonKeySet_clicked();
    void on_buttonTherapist_clicked();
    void on_buttonCondition_clicked();

    void WorkUpdate(QString results);
    void WorkFinished(DirectoryParse finalResult, ParseTypes::ParseAction action);

    void UpdateSessionCount(int session);

    void on_comboGroup_currentIndexChanged(int index);
    void on_comboIndividual_currentIndexChanged(int index);
    void on_comboEvaluation_currentIndexChanged(int index);
    void on_comboKeySet_currentIndexChanged(int index);
    void on_buttonCollector_clicked();

    void on_buttonBox_clicked(QAbstractButton *button);

    void SetGroups(QStringList Groups);
    void SetIndividuals(QStringList Individuals);
    void SetEvaluations(QStringList Evaluations, QStringList KeySets, QStringList Therapists, QStringList Collectors);
    void SetConditions(QStringList Conditions);

    void DefaultComboBox(QComboBox *mSelectedBox);

    void on_comboSessionDuration_currentIndexChanged(int index);

private:
    Ui::SessionWindow *ui;

    QString mWorkingDirectory;
    KeySet CurrentKeySet;

    QThread *workerThread;
    DirectorySearcher *worker;

    QThread *counterThread;
    SessionCounter *counter;

    DirectoryParse mCurrentDirectory;

    RecordingWindow r;
    ResultsDialog mResults;
};

#endif // SESSIONWINDOW_H
