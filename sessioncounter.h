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

#ifndef SESSIONCOUNTER_H
#define SESSIONCOUNTER_H

#include <QObject>

class SessionCounter : public QObject
{
    Q_OBJECT

public:
    SessionCounter(QString workingDirectory, QString group, QString individual, QString evaluation);

signals:
    void workStarted();
    void workingResult(const QString &value);
    void workFinished(const int value);

public slots:
    void startWork();
    void working();

private:
    QString currentDirectory;
    QString Group;
    QString Individual;
    QString Evaluation;

};

#endif // SESSIONCOUNTER_H
