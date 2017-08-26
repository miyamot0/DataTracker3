#ifndef SCORINGTOOLS_H
#define SCORINGTOOLS_H

#include <QObject>

#include "sessionevent.h"
#include "keyset.h"
#include "keysetentry.h"

class ScoringTools
{
public:

static void ScoreOverallSchedule(QList<SessionEvent> * PressedKeys, KeySet * currentKeySet, QDateTime * startTime, QDateTime * endTime, QList<QPair<QString, double> > * FrequencyKeys,
                                 QList<QPair<QString, double> > * DurationKeys, qint64 *timeMeasured)
{
    KeySetEntry temp;

    * timeMeasured = startTime->msecsTo(*endTime);

    FrequencyKeys->clear();

    int counter = 0;

    for(int i(0); i<currentKeySet->FrequencyKeys.count(); i++)
    {
        temp = currentKeySet->FrequencyKeys.at(i);

        counter = 0;

        for (int j(0); j<PressedKeys->count(); j++)
        {

            if (PressedKeys->at(j).KeyEntered.KeyCode == temp.KeyCode)
            {
                counter++;
            }
        }

        FrequencyKeys->append(QPair<QString, double>(temp.KeyDescription, counter));
    }

    qint64 msecCounter = 0;
    QDateTime prev, after;
    bool waitingForNext = false;

    DurationKeys->clear();

    for (int i(0); i<currentKeySet->DurationKeys.count(); i++)
    {
        temp = currentKeySet->DurationKeys.at(i);
        msecCounter = 0;
        waitingForNext = false;

        for (int j(0); j<PressedKeys->count(); j++)
        {
            if (PressedKeys->at(j).KeyEntered.KeyCode == temp.KeyCode)
            {
                if (waitingForNext)
                {
                    after = PressedKeys->at(j).TimePressed;

                    waitingForNext = false;

                    msecCounter = msecCounter + prev.msecsTo(after);
                }
                else
                {
                    prev = PressedKeys->at(j).TimePressed;

                    waitingForNext = true;
                }
            }
        }

        if (waitingForNext)
        {
            msecCounter = msecCounter + prev.msecsTo(*endTime);
        }

        DurationKeys->append(QPair<QString, double>(temp.KeyDescription, msecCounter));
    }
}

static int GetScheduleCode(Schedule schedule)
{
    if (schedule == Schedule::One)
    {
        return Qt::Key_Z;
    }
    else if (schedule == Schedule::Two)
    {
        return Qt::Key_X;
    }
    else
    {
        return Qt::Key_C;
    }
}

static qint64 GetScheduleTotalTime(QList<SessionEvent> * PressedKeys, Schedule schedule, QDateTime * endTime)
{
    bool isSchedule = false;
    qint64 msecCounter = 0;
    QDateTime prev, after;

    int mScheduleKey = GetScheduleCode(schedule);

    // Get Total Schedule Time
    for (int i(0); i<PressedKeys->count(); i++)
    {
        if (PressedKeys->at(i).KeyEntered.KeyCode == mScheduleKey)
        {
            if (isSchedule)
            {
                after = PressedKeys->at(i).TimePressed;

                isSchedule = false;

                msecCounter = msecCounter + prev.msecsTo(after);
            }
            else
            {
                prev = PressedKeys->at(i).TimePressed;

                isSchedule = true;
            }
        }
    }

    if (isSchedule)
    {
        msecCounter = msecCounter + prev.msecsTo(*endTime);
    }

    return msecCounter;
}

static void ScoreSpecificSchedule(QList<SessionEvent> * PressedKeys, KeySet * currentKeySet, QDateTime * endTime, Schedule schedule,
                                  QList<QPair<QString, double> > * FrequencyKeys, QList<QPair<QString, double> > * DurationKeys, qint64 * timeMeasured)
{
    FrequencyKeys->clear();
    DurationKeys->clear();

    KeySetEntry temp;
    int counter = 0;
    QDateTime prev, after;

    int mScheduleKey = GetScheduleCode(schedule);

    bool isInSchedule = false;

    * timeMeasured = GetScheduleTotalTime(PressedKeys, schedule, endTime);

    for(int i(0); i<currentKeySet->FrequencyKeys.count(); i++)
    {
        temp = currentKeySet->FrequencyKeys.at(i);

        counter = 0;

        for (int j(0); j<PressedKeys->count(); j++)
        {
            if (PressedKeys->at(j).KeyEntered.KeyCode == mScheduleKey)
            {
                isInSchedule = !isInSchedule;
            }
            else if (isInSchedule && PressedKeys->at(j).KeyEntered.KeyCode == temp.KeyCode)
            {
                counter++;
            }
        }

        FrequencyKeys->append(QPair<QString, double>(temp.KeyDescription, counter));
    }

    qint64 msecCounter = 0;
    bool waitingForNext;

    for (int i(0); i<currentKeySet->DurationKeys.count(); i++)
    {
        temp = currentKeySet->DurationKeys.at(i);

        waitingForNext = false;
        isInSchedule = false;
        msecCounter = 0;

        // enter key to open schedule

        // if were in schedule and key is pressed
        //      Set prior, flag waiting
        //      Set after, calculate time, unflag
        //      Repeate

        // enter key to close schedule

        for (int j(0); j<PressedKeys->count(); j++)
        {
            if (PressedKeys->at(j).KeyEntered.KeyCode == mScheduleKey)
            {
                // is laeving schedule
                if (isInSchedule)
                {
                    //no need to wait, just end it

                    if (waitingForNext)
                    {
                        after = PressedKeys->at(j).TimePressed;

                        waitingForNext = false;

                        msecCounter = msecCounter + prev.msecsTo(after);
                    }

                    isInSchedule = false;
                }
                // entering schedule
                else
                {

                    isInSchedule = true;
                }
            }
            else if (isInSchedule && PressedKeys->at(j).KeyEntered.KeyCode == temp.KeyCode)
            {
                if (waitingForNext)
                {
                    after = PressedKeys->at(j).TimePressed;

                    waitingForNext = false;

                    msecCounter = msecCounter + prev.msecsTo(after);
                }
                else
                {
                    prev = PressedKeys->at(j).TimePressed;

                    waitingForNext = true;
                }
            }

        }

        DurationKeys->append(QPair<QString, double>(temp.KeyDescription, msecCounter));
    }
}

};

#endif // SCORINGTOOLS_H
