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

#ifdef _WIN32
    SessionEvent& SessionEvent::operator=( const SessionEvent& other ) {
        KeyEntered = other.KeyEntered;
        TimePressed = other.TimePressed;
        ScheduleType = other.ScheduleType;
        MeasurementType = other.MeasurementType;

        return *this;
    }
#elif TARGET_OS_MAC
    SessionEvent& operator=( const SessionEvent& other ) {
        KeyEntered = other.KeyEntered;
        TimePressed = other.TimePressed;
        ScheduleType = other.ScheduleType;
        MeasurementType = other.MeasurementType;

        return *this;
    }
#endif


};

#endif // SESSIONEVENT_H
