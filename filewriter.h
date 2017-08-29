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

#ifndef FILEWRITER_H
#define FILEWRITER_H

#include <QObject>

#include "filetools.h"

class FileWriter : public QObject
{
    Q_OBJECT

public:
    FileWriter(QString workingDirectory,
               KeySet currentKeySet,
               QString group,
               QString individual,
               QString evaluation, QString condition, QString therapist,
               QString keySet,
               QString collector,
               QString role,
               QString startTime,
               QString endTime,
               qint64 timeOverall,
               qint64 timeOne,
               qint64 timeTwo,
               qint64 timeThree,
               QList<SessionEvent> pressedKeys,
               QList<QPair<QString, double> > *frequencyKeysOverall,
               QList<QPair<QString, double> > *durationKeysOverall,
               QList<QPair<QString, double> > *frequencyKeysOne,
               QList<QPair<QString, double> > *durationKeysOne,
               QList<QPair<QString, double> > *frequencyKeysTwo,
               QList<QPair<QString, double> > *durationKeysTwo,
               QList<QPair<QString, double> > *frequencyKeysThree,
               QList<QPair<QString, double> > *durationKeysThree,
               bool outputSpreadsheets,
               bool autoCalculateReli,
               QString alternativeSaveDirectory);

signals:
    void workStarted();
    void workFinished(const QString &value);

public slots:
    void startWork();
    void working();

private:
    QString WorkingDirectory;
    KeySet * CurrentKeySet;

    QString Group,
            Individual,
            Evaluation,
            Condition,
            Therapist,
            KeySetName,
            Collector,
            Role,
            StartTime,
            EndTime;

    QString AlternativeSaveLocation;

    qint64 TimeOverall,
           TimeOne,
           TimeTwo,
           TimeThree;

    bool OutputSheets,
         AutoCalculateReli;

    QList<QPair<QString,double>> FrequencyKeysOverall,
                                 DurationKeysOverall,
                                 FrequencyKeysOne,
                                 DurationKeysOne,
                                 FrequencyKeysTwo,
                                 DurationKeysTwo,
                                 FrequencyKeysThree,
                                 DurationKeysThree;

    QList<SessionEvent> PressedKeys;

    QList<QPair<QString, double>> CreateListCopy(QList<QPair<QString, double>> * original)
    {
        QList<QPair<QString, double>> copy;

        QPair<QString, double> *temp;

        for(QPair<QString, double> item : *original)
        {
            temp = new QPair<QString, double>(QString(item.first), double(item.second));

            copy.append(*temp);
        }

        return copy;
    }

    QList<SessionEvent> CreateKeyListCopy(QList<SessionEvent> original)
    {
        QList<SessionEvent> copy;

        SessionEvent *temp;

        for(SessionEvent item : original)
        {
            temp = new SessionEvent(item);

            copy.append(*temp);
        }

        return copy;

    }
};

#endif // FILEWRITER_H
