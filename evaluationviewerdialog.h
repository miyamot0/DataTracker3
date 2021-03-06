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

#ifndef EVALUATIONVIEWERDIALOG_H
#define EVALUATIONVIEWERDIALOG_H

#include <QDialog>
#include <QtCharts>

#include "directoryparse.h"
#include "directorysearcher.h"
#include "reliabilityparse.h"
#include "seriesselectdialog.h"
#include "displaytablesdialog.h"

namespace Ui {
class EvaluationViewerDialog;
}

class EvaluationViewerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EvaluationViewerDialog(QString mCurrentWorkingDirectory, QWidget *parent = 0);
    ~EvaluationViewerDialog();

public slots:
    void WorkUpdate(QString update);
    void WorkFinished(DirectoryParse finalResult, ParseTypes::ParseAction action);

    void DrawBlankPlot();
    void DrawFrequencyPlot();
    void DrawDurationPlot();

private slots:
    void on_comboBoxGroup_currentIndexChanged(int index);
    void on_comboBoxIndividual_currentIndexChanged(int index);
    void on_comboBoxEvaluation_currentIndexChanged(int index);
    void on_comboBoxDimension_currentIndexChanged(int index);
    void on_comboBoxSchedule_currentIndexChanged(int);

    void on_pushButton_clicked();

private:
    Ui::EvaluationViewerDialog *ui;

    SeriesSelectDialog mSeriesSelect;

    DisplayTablesDialog resultsDisplay;

    DirectoryParse mCurrentDirectory;
    QString mWorkingDirectory;
    QThread * workerThread;
    DirectorySearcher * worker;

    QList<ReliabilityParse> PrimaryReliabilityObjects;

    QJsonObject json;

    QChart * chart;
    QChartView * chartView;
    QValueAxis * axisX,
               * axisY;
    QList<QLineSeries *> lineSeries;
    QList<QScatterSeries *> pointSeries;
    QList<QString> fKeyDesc, dKeyDesc;
    QList<QString> fKeyConds, dKeyConds;

    QList<bool> fKeyShow, dKeyShow;

    void GetFrequencyKeySets();
    void GetDurationKeySets();

    QString createKeyString(QString base, int value)
    {
        QString mSuffix;

        if (value == 1)
        {
            mSuffix = "One";
        }
        else if (value == 2)
        {
            mSuffix = "Two";
        }
        else if (value == 3)
        {
            mSuffix = "Three";
        }
        else
        {
            mSuffix = "Overall";
        }

        return base.append(mSuffix);
    }
};

#endif // EVALUATIONVIEWERDIALOG_H
