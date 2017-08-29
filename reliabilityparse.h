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

#ifndef RELIABILITYPARSE_H
#define RELIABILITYPARSE_H

#include <QObject>

class ReliabilityParse
{

public:
    int SessionNumber;

    QString Condition;
    QString Collector;
    QString SecondaryObserver;

    QString PrimaryFilePath;
    QString ReliFilePath;

    bool Reli;

    int Row;

    bool CanScoreAsReli;

};

#endif // RELIABILITYPARSE_H
