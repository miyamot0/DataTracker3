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

#ifndef RESULTSDIALOG_H
#define RESULTSDIALOG_H

#include "sessionevent.h"
#include "keyset.h"

#include <QDialog>
#include <QtCharts>

namespace Ui {
class ResultsDialog;
}

class ResultsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ResultsDialog(QWidget *parent = 0);

    qint64 TimeOverall;
    qint64 TimeOne;
    qint64 TimeTwo;
    qint64 TimeThree;

    QList<QPair<QString, double>> FrequencyOverall, DurationOverall;
    QList<QPair<QString, double>> FrequencyOne, DurationOne;
    QList<QPair<QString, double>> FrequencyTwo, DurationTwo;
    QList<QPair<QString, double>> FrequencyThree, DurationThree;

    void BuildTables();
    void BuildPlot(KeySet currKeySet, QList<SessionEvent> * PressedKeys, QDateTime * startTime, QDateTime *endTime);
    void BuildNarrative(QList<SessionEvent> *PressedKeys, QDateTime *startTime);

    void SetTabEnabled(int index, bool value);
    void SetParameters(QString group, QString individual, QString evaluation,
                       QString condition, QString therapist, QString keySet,
                       QString collector, QString role);
    void SetKeySet(KeySet currKeySet);

    ~ResultsDialog();

private:
    Ui::ResultsDialog *ui;

    QChart * chart,
           * chart2;

    QChartView * chartView,
               * chartView2;

    QValueAxis * axisX,
               * axisY,
               * axisX2,
               * axisY2;

    QList<QLineSeries *> lineSeries,
                         lineSeries2;

    KeySet CurrentKeySet;

    QString Group,
            Individual,
            Evaluation,
            Condition,
            Therapist,
            KeySetName,
            Collector,
            Role;

    bool Drawn = false;

    QString formatTimeLabel(int msecs)
    {
        int msecMod = msecs % 1000;
        int secs = msecs / 1000;
        int mins = (secs / 60) % 60;
        int hours = (secs / 3600);
        secs = secs % 60;

        return QString("%1:%2:%3:%4")
        .arg(hours, 2, 10, QLatin1Char('0'))
        .arg(mins, 2, 10, QLatin1Char('0'))
        .arg(secs, 2, 10, QLatin1Char('0'))
        .arg(msecMod, 2, 10, QLatin1Char('0'));
    }

    QString formatRate(double count, qint64 totalTime)
    {
        if (totalTime == 0)
        {
            return QString("---");
        }
        else
        {
            return QString::number(((count) / ((double) totalTime / 60000)), 'f', 2);
        }
    }

    QString formatPercentage(double percent, qint64 totalTime)
    {
        if (totalTime == 0)
        {
            return QString("---");
        }
        else
        {
            return QString(QString::number((percent / ((double) totalTime)) * 100, 'f', 2) + "%");
        }
    }
};

#endif // RESULTSDIALOG_H
