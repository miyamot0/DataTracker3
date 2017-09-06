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

#ifndef SEQUENTIALANALYSISDIALOG_H
#define SEQUENTIALANALYSISDIALOG_H

#include <QDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QThread>

#include "directoryparse.h"
#include "parsetypes.h"
#include "seriesselectdialog.h"
#include "directorysearcher.h"
#include "reliabilityparse.h"

#include "keysetentry.h"
#include "sessionevent.h"

namespace Ui {
class SequentialAnalysisDialog;
}

class SequentialAnalysisDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SequentialAnalysisDialog(QString mCurrentWorkingDirectory, QWidget *parent = 0);
    ~SequentialAnalysisDialog();

private slots:
    void on_comboBoxGroup_currentIndexChanged(int index);
    void on_comboBoxIndividual_currentIndexChanged(int index);
    void on_comboBoxEvaluation_currentIndexChanged(int index);
    void on_comboBoxAnalysis_currentIndexChanged(int index);
    void on_comboBoxWindow_currentIndexChanged(int index);

    void on_tableWidget_currentCellChanged(int rowChanged, int, int, int);

    void WorkUpdate(QString update);
    void WorkFinished(DirectoryParse finalResult, ParseTypes::ParseAction action);

    void GetKeys();
    void ChartYule(int index);
    int GetWindowSpan();
    QColor GetColorMapping(QString value);

private:
    Ui::SequentialAnalysisDialog *ui;

    SeriesSelectDialog mSeriesSelect;

    DirectoryParse mCurrentDirectory;
    QString mWorkingDirectory;
    QThread *workerThread;
    DirectorySearcher *worker;

    QList<ReliabilityParse> PrimaryReliabilityObjects;

    QJsonObject json;

    ReliabilityParse temp;
    bool result;

    QDateTime startTime, endTime;

    int totalSecs;

    QList<KeySetEntry> FrequencyKeys;
    QJsonArray frequencyArray;

    QList<KeySetEntry> DurationKeys;
    QJsonArray durationArray;

    QList<SessionEvent> PressedKeys;
    QJsonArray pressedKeysJson;

    QStringList keyList;
    QList<bool> keyShowList;
    QList<QPair<QString, int>> mScoreKey;

};

#endif // SEQUENTIALANALYSISDIALOG_H
