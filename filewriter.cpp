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

#include <QObject>

#include "filewriter.h"
#include "reliabilityscoring.h"
#include "filetools.h"
#include "keyset.h"

FileWriter::FileWriter(QString workingDirectory,
                       KeySet currentKeySet,
                       QString group,
                       QString individual,
                       QString evaluation,
                       QString condition,
                       QString therapist,
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
                       QList<QPair<QString,double>> * frequencyKeysOverall,
                       QList<QPair<QString,double>> * durationKeysOverall,
                       QList<QPair<QString,double>> * frequencyKeysOne,
                       QList<QPair<QString,double>> * durationKeysOne,
                       QList<QPair<QString,double>> * frequencyKeysTwo,
                       QList<QPair<QString,double>> * durationKeysTwo,
                       QList<QPair<QString,double>> * frequencyKeysThree,
                       QList<QPair<QString,double>> * durationKeysThree,
                       bool outputSpreadsheets,
                       bool autoCalculateReli,
                       QString alternativeSaveDirectory)
{
    WorkingDirectory = QString(workingDirectory);
    CurrentKeySet = new KeySet(currentKeySet);
    Group = QString(group);
    Individual = QString(individual);
    Evaluation = QString(evaluation);
    Condition = QString(condition);
    Therapist = QString(therapist);
    KeySetName = QString(keySet);
    Collector = QString(collector);
    Role = QString(role);
    StartTime = QString(startTime);
    EndTime = QString(endTime);

    AlternativeSaveLocation = QString(alternativeSaveDirectory);

    OutputSheets = bool(outputSpreadsheets);
    AutoCalculateReli = bool(autoCalculateReli);

    TimeOverall = timeOverall;
    TimeOne = timeOne;
    TimeTwo = timeTwo;
    TimeThree = timeThree;

    PressedKeys = CreateKeyListCopy(pressedKeys);

    FrequencyKeysOverall = CreateListCopy(frequencyKeysOverall);
    DurationKeysOverall = CreateListCopy(durationKeysOverall);
    FrequencyKeysOne = CreateListCopy(frequencyKeysOne);
    DurationKeysOne = CreateListCopy(durationKeysOne);
    FrequencyKeysTwo = CreateListCopy(frequencyKeysTwo);
    DurationKeysTwo = CreateListCopy(durationKeysTwo);
    FrequencyKeysThree = CreateListCopy(frequencyKeysThree);
    DurationKeysThree = CreateListCopy(durationKeysThree);
}

void FileWriter::startWork()
{
    emit workStarted();
}

void FileWriter::working()
{
    bool runStatus, altRunStatus;
    QString reliMsg = "";

    runStatus = FileTools::WriteSessionJSON(WorkingDirectory,
                                *CurrentKeySet,
                                Group,
                                Individual,
                                Evaluation,
                                Condition,
                                Therapist,
                                KeySetName,
                                Collector,
                                Role,
                                StartTime,
                                EndTime,
                                TimeOverall,
                                TimeOne,
                                TimeTwo,
                                TimeThree,
                                &PressedKeys,
                                &FrequencyKeysOverall,
                                &DurationKeysOverall,
                                &FrequencyKeysOne,
                                &DurationKeysOne,
                                &FrequencyKeysTwo,
                                &DurationKeysTwo,
                                &FrequencyKeysThree,
                                &DurationKeysThree);


    if (OutputSheets)
    {
        runStatus = FileTools::WriteSessionSpreadsheet(WorkingDirectory,
                                           *CurrentKeySet,
                                           Group,
                                           Individual,
                                           Evaluation,
                                           Condition,
                                           Therapist,
                                           KeySetName,
                                           Collector,
                                           Role,
                                           StartTime,
                                           TimeOverall,
                                           TimeOne,
                                           TimeTwo,
                                           TimeThree,
                                           &PressedKeys,
                                           &FrequencyKeysOverall,
                                           &DurationKeysOverall,
                                           &FrequencyKeysOne,
                                           &DurationKeysOne,
                                           &FrequencyKeysTwo,
                                           &DurationKeysTwo,
                                           &FrequencyKeysThree,
                                           &DurationKeysThree);
    }

    if (QDir(AlternativeSaveLocation).exists())
    {
        altRunStatus = FileTools::WriteSessionJSON(AlternativeSaveLocation,
                                    *CurrentKeySet,
                                    Group,
                                    Individual,
                                    Evaluation,
                                    Condition,
                                    Therapist,
                                    KeySetName,
                                    Collector,
                                    Role,
                                    StartTime,
                                    EndTime,
                                    TimeOverall,
                                    TimeOne,
                                    TimeTwo,
                                    TimeThree,
                                    &PressedKeys,
                                    &FrequencyKeysOverall,
                                    &DurationKeysOverall,
                                    &FrequencyKeysOne,
                                    &DurationKeysOne,
                                    &FrequencyKeysTwo,
                                    &DurationKeysTwo,
                                    &FrequencyKeysThree,
                                    &DurationKeysThree);

        if (OutputSheets)
        {
            altRunStatus = FileTools::WriteSessionSpreadsheet(AlternativeSaveLocation,
                                               *CurrentKeySet,
                                               Group,
                                               Individual,
                                               Evaluation,
                                               Condition,
                                               Therapist,
                                               KeySetName,
                                               Collector,
                                               Role,
                                               StartTime,
                                               TimeOverall,
                                               TimeOne,
                                               TimeTwo,
                                               TimeThree,
                                               &PressedKeys,
                                               &FrequencyKeysOverall,
                                               &DurationKeysOverall,
                                               &FrequencyKeysOne,
                                               &DurationKeysOne,
                                               &FrequencyKeysTwo,
                                               &DurationKeysTwo,
                                               &FrequencyKeysThree,
                                               &DurationKeysThree);
        }

        reliMsg = (altRunStatus) ? "Backup save successful" : "Backup save failed";
    }

    if (AutoCalculateReli)
    {
        runStatus = ReliabilityScoring::PerformReliabilityCheck(WorkingDirectory,
                                                    Group,
                                                    Individual,
                                                    Evaluation);
    }

    QString primary = (runStatus) ? "Primary save successful" : "Primary save failed";

    if (QDir(AlternativeSaveLocation).exists())
    {
        primary.append(", " + reliMsg);
    }

    QString mResponse = QString("Results: Session %1 (%2)")
                        .arg(CurrentKeySet->Session)
                        .arg(primary);

    emit workFinished(mResponse);
}
