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

    QDateTime startTime, endTime;

    QList<SessionEvent> PressedKeys;

    QList<bool> DurationFlags;
    QList<qint64> DurationSums;
    QList<QDateTime> DurationFlaggedTimes;

    QList<bool> ScheduleFlags;
    QList<qint64> ScheduleDurationSums;
    QList<QDateTime> ScheduleDurationFlaggedTimes;

    QString formatTimeLabel(int msecs);

    Schedule CurrentSchedule;

    void DetectScheduleKey(QKeyEvent * mKey);
    void DetectFrequencyKey(QKeyEvent * mKey);
    void DetectDurationKey(QKeyEvent * mKey);

    void AddKey(SessionEvent pressedKey);
    void RemoveKey();

    void UpdateTables();

    void ParseTimes();

    bool Started = false;

};

#endif // RECORDINGWINDOW_H
