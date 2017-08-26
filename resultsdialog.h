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
    void BuildJson(QList<SessionEvent> * PressedKeys, QDateTime * startTime, QString mWorkingDirectory);

    void SetParameters(QString group, QString individual, QString evaluation,
                       QString condition, QString therapist, QString keySet,
                       QString collector, QString role);
    void SetKeySet(KeySet currKeySet);

    ~ResultsDialog();

private:
    Ui::ResultsDialog *ui;

    QChart chart, chart2;
    QChartView *chartView, *chartView2;
    QValueAxis axisX, axisY, axisX2, axisY2;
    QList<QLineSeries *> lineSeries, lineSeries2;

    KeySet CurrentKeySet;

    QString Group;
    QString Individual;
    QString Evaluation;
    QString Condition;
    QString Therapist;
    QString KeySetName;
    QString Collector;
    QString Role;

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

    QString formatSchedule(Schedule schedule)
    {
        if (schedule == Schedule::One)
        {
            return QString("Schedule 1");
        }
        else if (schedule == Schedule::Two)
        {
            return QString("Schedule 2");
        }
        else
        {
            return QString("Schedule 3");
        }
    }

    QString formatMeasurement(Measurement measurement)
    {
        if (measurement == Measurement::Rate)
        {
            return QString("Rate");
        }
        else if (measurement == Measurement::Time)
        {
            return QString("Time");
        }
        else
        {
            return QString("Schedule");
        }
    }
};

#endif // RESULTSDIALOG_H
