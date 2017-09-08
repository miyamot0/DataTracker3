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

#ifndef SEQUENTIALCONDITIONANALYSISDIALOG_H
#define SEQUENTIALCONDITIONANALYSISDIALOG_H

#include <QDialog>
#include <QThread>
#include <QJsonObject>
#include <QJsonArray>

#include "directoryparse.h"
#include "directorysearcher.h"
#include "reliabilityparse.h"
#include "sessionevent.h"
#include "seriesselectdialog.h"

namespace Ui {
class SequentialConditionAnalysisDialog;
}

class SequentialConditionAnalysisDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SequentialConditionAnalysisDialog(QString mCurrentWorkingDirectory, QWidget *parent = 0);
    ~SequentialConditionAnalysisDialog();

private slots:

    void WorkUpdate(QString update);
    void WorkFinished(DirectoryParse finalResult, ParseTypes::ParseAction action);

    void on_comboBoxGroup_currentIndexChanged(int index);
    void on_comboBoxIndividual_currentIndexChanged(int index);
    void on_comboBoxEvaluation_currentIndexChanged(int index);
    void on_comboBoxCondition_currentIndexChanged(int index);
    void on_comboBoxWindowSize_currentIndexChanged(int index);

    void on_pushButton_clicked();

    void GetKeys();
    void ChartYule();
    int GetWindowSpan();
    QColor GetColorMapping(QString value);

private:
    Ui::SequentialConditionAnalysisDialog *ui;

    SeriesSelectDialog mSeriesSelect;

    DirectoryParse mCurrentDirectory;
    QString mWorkingDirectory;
    QThread * workerThread;
    DirectorySearcher * worker;

    QList<ReliabilityParse> PrimaryReliabilityObjects;
    QList<ReliabilityParse> SecondaryReliabilityObjects;

    QList<QList<QStringList>> ReportResults;

    QJsonObject json;

    ReliabilityParse temp;
    bool result;

    QList<SessionEvent> PressedKeys;
    QJsonArray pressedKeysJson;

    QStringList keyList;
    QList<bool> keyShowList;
    QList<QPair<QString, int>> mScoreKey;
};

#endif // SEQUENTIALCONDITIONANALYSISDIALOG_H
