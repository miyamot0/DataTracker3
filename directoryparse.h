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

#ifndef DIRECTORYPARSE_H
#define DIRECTORYPARSE_H

#include <QWidget>

class DirectoryParse
{
public:
    QString WorkingDirectory;

    QString CurrentGroup;
    QString CurrentIndividual;
    QString CurrentEvaluation;
    QString CurrentCondition;
    QString CurrentKeySet;
    QString CurrentTherapist;
    QString CurrentCollector;

    QStringList Groups;
    QStringList Individuals;
    QStringList Evaluations;
    QStringList Conditions;
    QStringList KeySets;
    QStringList Therapists;
    QStringList Collectors;

    bool Primary;
    int Duration;
    int Session;
};

#endif // DIRECTORYPARSE_H
