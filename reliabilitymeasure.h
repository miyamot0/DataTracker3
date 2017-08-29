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

#ifndef RELIABILITYMEASURE_H
#define RELIABILITYMEASURE_H

#include <QObject>
#include "keysetentry.h"

class ReliabilityMeasure
{
public:
    int Session;
    QString Group;
    QString Individual;
    QString Evaluation;
    QString Condition;

    QString Primary;
    QString Reliability;

    QList<KeySetEntry> FrequencyKeys;

    QList<QPair<QString, QString>> fEIA;
    QList<QPair<QString, QString>> fPIA;
    QList<QPair<QString, QString>> fTIA;
    QList<QPair<QString, QString>> fOIA;
    QList<QPair<QString, QString>> fNIA;
    QList<QPair<QString, QString>> fPMA;

    QList<KeySetEntry> DurationKeys;

    QList<QPair<QString, QString>> dEIA;
    QList<QPair<QString, QString>> dPIA;
    QList<QPair<QString, QString>> dTIA;
    QList<QPair<QString, QString>> dOIA;
    QList<QPair<QString, QString>> dNIA;
    QList<QPair<QString, QString>> dPMA;

};

#endif // RELIABILITYMEASURE_H
