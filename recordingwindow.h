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

    bool eventFilter(QObject *, QEvent *e);

    void AddKey(SessionEvent pressedKey);
    void RemoveKey();

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

    QString formatTimeLabel(int msecs);

    void DetectFrequencyKey(QKeyEvent *mKey);
    void DetectDurationKey(QKeyEvent * mKey);

    void UpdateTables();

    void ParseTimes();

};

#endif // RECORDINGWINDOW_H
