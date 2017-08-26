#ifndef RESULTSDIALOG_H
#define RESULTSDIALOG_H

#include "sessionevent.h"

#include <QDialog>

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
    void BuildNarrative(QList<SessionEvent> *PressedKeys, QDateTime *startTime);

    ~ResultsDialog();

private:
    Ui::ResultsDialog *ui;

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
