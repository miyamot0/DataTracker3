#ifndef RECORDINGWINDOW_H
#define RECORDINGWINDOW_H

#include <QDialog>
#include <QElapsedTimer>
#include <QTime>
#include <QTimer>
#include <QDateTime>

#include "keyset.h"
#include "sessionevent.h"

namespace Ui {
class RecordingWindow;
}

class RecordingWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RecordingWindow(QWidget *parent = 0);
    ~RecordingWindow();

    KeySet keySet;
    QDateTime startTime, endTime;
    QList<SessionEvent> PressedKeys;

    void LoadKeys(KeySet mKeyset);
    void SetGroup(QString value);
    void SetIndividual(QString value);
    void SetEvaluation(QString value);
    void SetCondition(QString value);
    void SetCollector(QString value);
    void SetRole(QString value);

    bool eventFilter(QObject *, QEvent *e);

public slots:
    void UpdateGUI();

private:
    Ui::RecordingWindow *ui;

    QTimer baseTimer;

    QList<bool> DurationFlags;
    QList<qint64> DurationSums;
    QList<QDateTime> DurationFlaggedTimes;

    QList<bool> ScheduleFlags;
    QList<qint64> ScheduleDurationSums;
    QList<QDateTime> ScheduleDurationFlaggedTimes;

    Schedule CurrentSchedule;

    void DetectScheduleKey(QKeyEvent * mKey);
    void DetectFrequencyKey(QKeyEvent * mKey);
    void DetectDurationKey(QKeyEvent * mKey);

    void AddKey(SessionEvent pressedKey);
    void RemoveKey();

    void UpdateTables();

    void ParseTimes();

    bool Started = false;

    void reject();

    QString formatScheduleString(Schedule schedule)
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

};

#endif // RECORDINGWINDOW_H
