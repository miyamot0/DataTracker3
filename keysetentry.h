/**
   Copyright 2017 Shawn Gilroy

   This file is part of Data Tracker, Qt port.

   Discounting Model Selector is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 3.

   Discounting Model Selector is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Discounting Model Selector.  If not, see http://www.gnu.org/licenses/.

   The Discounting Model Selector is a tool to assist researchers in behavior economics.

   Email: shawn(dot)gilroy(at)temple.edu

  */

#ifndef KEYSETENTRY_H
#define KEYSETENTRY_H

#include <QObject>

class KeySetEntry
{
public:
    QString KeyName;
    QString KeyDescription;

    int KeyCode;
};

#endif // KEYSETENTRY_H
