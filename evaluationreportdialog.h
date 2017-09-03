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

#ifndef EVALUATIONREPORTDIALOG_H
#define EVALUATIONREPORTDIALOG_H

#include <QDialog>
#include <QJsonObject>
#include <QTimer>

#include "reliabilityparse.h"
#include "directoryparse.h"
#include "directorysearcher.h"
#include "displaytablesdialog.h"

namespace Ui {
class EvaluationReportDialog;
}

class EvaluationReportDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EvaluationReportDialog(QString mCurrentWorkingDirectory, QWidget *parent = 0);
    ~EvaluationReportDialog();

private slots:
    void on_comboBoxGroup_currentIndexChanged(int index);
    void on_comboBoxIndividual_currentIndexChanged(int index);
    void on_comboBoxEvaluation_currentIndexChanged(int index);
    void on_pushButton_clicked();

    void WorkUpdate(QString update);
    void WorkFinished(DirectoryParse finalResult, ParseTypes::ParseAction action);

    void ClearStatus();

private:
    Ui::EvaluationReportDialog *ui;

    DirectoryParse mCurrentDirectory;
    QString mWorkingDirectory;
    QThread * workerThread;
    DirectorySearcher * worker;

    QList<ReliabilityParse> PrimaryReliabilityObjects;
    QList<ReliabilityParse> SecondaryReliabilityObjects;

    DisplayTablesDialog mDisplayDialog;

    QList<QList<QStringList>> ReportResults;

    QJsonObject json;
    QTimer fadeTimer;
};

#endif // EVALUATIONREPORTDIALOG_H
