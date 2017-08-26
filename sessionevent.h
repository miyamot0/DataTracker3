#ifndef SESSIONEVENT_H
#define SESSIONEVENT_H

#include <QObject>
#include <QDateTime>

#include "keysetentry.h"

enum class Schedule
{
    One,
    Two,
    Three
};

enum class Measurement
{
    Rate,
    Time,
    Schedule
};

class SessionEvent
{
public:
    KeySetEntry KeyEntered;
    QDateTime TimePressed;
    Schedule ScheduleType;
    Measurement MeasurementType;



};

#endif // SESSIONEVENT_H
