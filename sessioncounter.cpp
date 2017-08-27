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

#include <QDebug>
#include <QObject>
#include <QDir>
#include <QDirIterator>

#include "sessioncounter.h"
#include "filetools.h"

SessionCounter::SessionCounter(QString workingDirectory, QString group, QString individual, QString evaluation)
{
    currentDirectory = workingDirectory;
    Group = group;
    Individual = individual;
    Evaluation = evaluation;
}

void SessionCounter::startWork()
{
    emit workStarted();
}

void SessionCounter::working()
{
    QString mFilePath = FileTools::pathAppend(currentDirectory, Group);
    mFilePath = FileTools::pathAppend(mFilePath, Individual);
    mFilePath = FileTools::pathAppend(mFilePath, Evaluation);

    int value;
    int mSessionNumbering = 0;

    QDirIterator iterator(mFilePath,
                          QStringList() << "*.json",
                          QDir::Files,
                          QDirIterator::Subdirectories);

    while (iterator.hasNext())
    {
        QString mFileName = iterator.next();

        if (mFileName.contains(".json", Qt::CaseInsensitive))
        {
            value = FileTools::ReadSessionJSONint(mFileName);

            if (value > mSessionNumbering)
            {
                mSessionNumbering = value;
            }

        }
     }

    mSessionNumbering++;

    emit workFinished(mSessionNumbering);
}
