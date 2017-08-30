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

#ifndef FILETOOLS_H
#define FILETOOLS_H

#include <QStandardPaths>
#include <QObject>
#include <QDir>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <QtXlsx>

#include "directoryparse.h"
#include "parsetypes.h"
#include "keyset.h"
#include "keysetentry.h"
#include "sessionevent.h"
#include "reliabilitymeasure.h"
#include "reliabilityparse.h"

class FileTools
{
public:

static bool ReadToLocal(QString workingDirectory, QString backupSaveLocation, QString folderTitle)
{
    bool value = true;

    QDirIterator iteratorD(workingDirectory,
                          QStringList() << "*",
                          QDir::Dirs | QDir::NoDotAndDotDot,
                          QDirIterator::Subdirectories);

    QString tempTest, base;

    while (iteratorD.hasNext())
    {
        base = QString(iteratorD.next());
        tempTest = base.replace(pathAppend(workingDirectory, folderTitle), backupSaveLocation);

        if (!QDir(tempTest).exists())
        {
            QDir().mkpath(tempTest);
        }
    }

    workingDirectory = pathAppend(workingDirectory, folderTitle);

    QStringList mFiles;
    QDirIterator iterator(workingDirectory,
                          QStringList() << "*.json",
                          QDir::Files,
                          QDirIterator::Subdirectories);

    while (iterator.hasNext())
    {
        mFiles << iterator.next();
    }

    QString repl = workingDirectory;

    QFile *tempFile;
    QFileInfo *tempFileInfo;

    foreach (QString mRemote, mFiles)
    {
        base = QString(mRemote);
        tempTest = mRemote.replace(workingDirectory, backupSaveLocation);

        tempFileInfo = new QFileInfo(tempTest);
        tempFile = new QFile(tempTest);

        if (!tempFile->exists())
        {
            QDir().mkpath(tempFileInfo->absolutePath());
            value = QFile::copy(base, tempTest);

            if (!value)
            {
                return false;
            }
        }
    }

    return true;
}

static bool WriteToRemote(QString workingDirectory, QString backupSaveLocation, QString folderTitle)
{
    bool value = true;

    QDirIterator iteratorD(workingDirectory,
                          QStringList() << "*",
                          QDir::Dirs | QDir::NoDotAndDotDot,
                          QDirIterator::Subdirectories);

    QString tempTest, base;

    while (iteratorD.hasNext())
    {
        QString mHolder = iteratorD.next();
        base = QString(mHolder);
        tempTest = base.replace(workingDirectory, FileTools::pathAppend(backupSaveLocation, folderTitle));

        if (!QDir(tempTest).exists())
        {
            QDir().mkpath(tempTest);
        }
    }

    QStringList mFiles;
    QDirIterator iterator(workingDirectory,
                          QStringList() << "*.json",
                          QDir::Files,
                          QDirIterator::Subdirectories);

    while (iterator.hasNext())
    {
        mFiles << iterator.next();
    }

    QString repl = FileTools::pathAppend(backupSaveLocation, folderTitle);

    QFile *tempFile;
    QFileInfo *tempFileInfo;

    foreach (QString mLocal, mFiles)
    {
        base = QString(mLocal);
        tempTest = mLocal.replace(workingDirectory, repl);

        tempFileInfo = new QFileInfo(tempTest);
        tempFile = new QFile(tempTest);

        if (!tempFile->exists())
        {
            QDir().mkpath(tempFileInfo->absolutePath());
            value = QFile::copy(base, tempTest);

            if (!value)
            {
                return false;
            }
        }
    }

    return true;
}

/** Read keys in from file
 * @brief ReadKeySet
 * @param path
 * @param keySet
 */
static void ReadKeySet(QString path, KeySet * keySet)
{
    QFile mKeySet(path);

    if (mKeySet.exists())
    {
        if (mKeySet.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString keySetData = mKeySet.readAll();
            mKeySet.close();

            QJsonDocument loadKeySets = QJsonDocument::fromJson(keySetData.toUtf8());
            QJsonObject  keySetObject = loadKeySets.object();

            keySet->KeySetName = keySetObject["Title"].toString();

            QJsonArray frequencyArray = keySetObject["FrequencyKeys"].toArray();

            for (int i=0; i<frequencyArray.size(); i++)
            {
                QJsonObject keyObject = frequencyArray[i].toObject();

                KeySetEntry newKeys;
                newKeys.KeyCode = keyObject["Code"].toInt();
                newKeys.KeyName = keyObject["Name"].toString();
                newKeys.KeyDescription = keyObject["Description"].toString();

                keySet->FrequencyKeys.append(newKeys);

            }

            QJsonArray durationArray = keySetObject["DurationKeys"].toArray();

            for (int i=0; i<durationArray.size(); i++)
            {
                QJsonObject keyObject = durationArray[i].toObject();

                KeySetEntry newKeys;
                newKeys.KeyCode = keyObject["Code"].toInt();
                newKeys.KeyName = keyObject["Name"].toString();
                newKeys.KeyDescription = keyObject["Description"].toString();

                keySet->DurationKeys.append(newKeys);

            }
        }
    }
}

/** Write key set to JSON file
 * @brief WriteKeySet
 * @param path
 * @param keySet
 */
static void WriteKeySet(QString path, KeySet keySet)
{
    QJsonObject json;
    json["Title"] = keySet.KeySetName;

    QJsonArray frequencyKeys;
    foreach(KeySetEntry entry, keySet.FrequencyKeys)
    {
        QJsonObject mEntry;
        mEntry["Code"] = entry.KeyCode;
        mEntry["Name"] = entry.KeyName;
        mEntry["Description"] = entry.KeyDescription;

        frequencyKeys.append(mEntry);
    }
    json["FrequencyKeys"] = frequencyKeys;

    QJsonArray durationKeys;
    foreach(KeySetEntry entry, keySet.DurationKeys)
    {
        QJsonObject mEntry;
        mEntry["Code"] = entry.KeyCode;
        mEntry["Name"] = entry.KeyName;
        mEntry["Description"] = entry.KeyDescription;

        durationKeys.append(mEntry);
    }
    json["DurationKeys"] = durationKeys;

    QJsonDocument jsonDoc(json);

    QFile saveFile(path);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        // TODO error handling
        return;
    }

    saveFile.write(jsonDoc.toJson());
}

/** Read therapists from file
 * @brief ReadTherapists
 * @param path
 * @param therapists
 */
static void ReadTherapists(QString path, QStringList * therapists)
{
    QFile mTherapists(path);

    if (mTherapists.exists())
    {
        if (mTherapists.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString therapistData = mTherapists.readAll();
            mTherapists.close();

            QJsonDocument loadTherapists = QJsonDocument::fromJson(therapistData.toUtf8());
            QJsonObject  therapistObject = loadTherapists.object();
            QJsonArray therapistArray = therapistObject["Therapists"].toArray();

            foreach (const QJsonValue therapist, therapistArray) {
                *therapists << therapist.toString();
            }
        }
    }
}

/** Write therapists to JSON file
 * @brief WriteTherapists
 * @param path
 * @param therapists
 */
static void WriteTherapists(QString path, QStringList therapists)
{
    QJsonObject json;

    QJsonArray therapistObj;

    foreach(QString entry, therapists)
    {
        therapistObj.append(entry);
    }
    json["Therapists"] = therapistObj;

    QJsonDocument jsonDoc(json);

    QFile saveFile(path);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        // TODO error handling

        return;
    }

    saveFile.write(jsonDoc.toJson());
}

/** Read collectors from file
 * @brief ReadCollectors
 * @param path
 * @param collectors
 */
static void ReadCollectors(QString path, QStringList * collectors)
{
    QFile mCollectors(path);

    if (mCollectors.exists())
    {
        if (mCollectors.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString collectorData = mCollectors.readAll();
            mCollectors.close();

            QJsonDocument loadCollectors = QJsonDocument::fromJson(collectorData.toUtf8());
            QJsonObject  collectorObject = loadCollectors.object();
            QJsonArray collectorArray = collectorObject["Collectors"].toArray();

            foreach (const QJsonValue collector, collectorArray) {
                *collectors << collector.toString();
            }
        }
    }
}

/** Write collectors to JSON file
 * @brief WriteCollectors
 * @param path
 * @param collectors
 */
static void WriteCollectors(QString path, QStringList collectors)
{
    QJsonObject json;

    QJsonArray collectorObj;

    foreach(QString entry, collectors)
    {
        collectorObj.append(entry);
    }
    json["Collectors"] = collectorObj;

    QJsonDocument jsonDoc(json);

    QFile saveFile(path);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        // TODO error handling

        return;
    }

    saveFile.write(jsonDoc.toJson());
}

/** Read directories and parse for session
 * @brief ReadSessionJSONint
 * @param path
 * @return
 */
static int ReadSessionJSONint(QString path)
{
    QFile mSession(path);

    if (mSession.exists())
    {
        if (mSession.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QString sessionData = mSession.readAll();
            mSession.close();

            QJsonDocument loadSession = QJsonDocument::fromJson(sessionData.toUtf8());
            QJsonObject sessionObject = loadSession.object();

            return sessionObject["Session"].toInt();
        }
        else
        {
            return -1;
        }
    }
    else
    {
        return -1;
    }
}

/**
 * @brief ReadSessionFromJSON
 * @param path
 * @param json
 * @return
 */
static bool ReadSessionFromJSON(QString path, QJsonObject * json)
{
    QFile mSession(path);

    if (mSession.exists())
    {
        if (mSession.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QString sessionData = mSession.readAll();

            mSession.close();

            QJsonDocument loadSession = QJsonDocument::fromJson(sessionData.toUtf8());

            * json = loadSession.object();

            return true;
        }
        else
        {
            return false;
        }
    }
    else
    {
        return false;
    }
}

/**
 * @brief formatRate
 * @param count
 * @param totalTime
 * @return
 */
static QString formatRate(double count, qint64 totalTime)
{
    if (totalTime == 0)
    {
        return QString("---");
    }
    else
    {
        return QString::number(((count) / ((double) totalTime / 60000)), 'f', 2);
    }
}

/**
 * @brief formatPercentage
 * @param percent
 * @param totalTime
 * @return
 */
static QString formatPercentage(double percent, qint64 totalTime)
{
    if (totalTime == 0)
    {
        return QString("---");
    }
    else
    {
        return QString(QString::number((percent / ((double) totalTime)) * 100, 'f', 2) + "%");
    }
}

/**
 * @brief WriteSessionJSON
 * @param mWorkingDirectory
 * @param CurrentKeySet
 * @param Group
 * @param Individual
 * @param Evaluation
 * @param Condition
 * @param Therapist
 * @param KeySetName
 * @param Collector
 * @param Role
 * @param StartTime
 * @param TimeOverall
 * @param TimeOne
 * @param TimeTwo
 * @param TimeThree
 * @param PressedKeys
 */
static bool WriteSessionJSON(QString mWorkingDirectory, KeySet CurrentKeySet, QString Group, QString Individual,
                             QString Evaluation, QString Condition, QString Therapist,
                             QString KeySetName, QString Collector, QString Role,
                             QString StartTime, QString EndTime, qint64 TimeOverall, qint64 TimeOne,
                             qint64 TimeTwo, qint64 TimeThree, QList<SessionEvent> *PressedKeys,
                             QList<QPair<QString,double>> * FrequencyKeysOverall, QList<QPair<QString,double>> * DurationKeysOverall,
                             QList<QPair<QString,double>> * FrequencyKeysOne, QList<QPair<QString,double>> * DurationKeysOne,
                             QList<QPair<QString,double>> * FrequencyKeysTwo, QList<QPair<QString,double>> * DurationKeysTwo,
                             QList<QPair<QString,double>> * FrequencyKeysThree, QList<QPair<QString,double>> * DurationKeysThree)
{
    QJsonObject json;

    // Demographics
    json["Session"] = CurrentKeySet.Session;
    json["Group"] = Group;
    json["Individual"] = Individual;
    json["Evaluation"] = Evaluation;
    json["Condition"] = Condition;
    json["Therapist"] = Therapist;
    json["KeySet"] = KeySetName;
    json["Collector"] = Collector;
    json["Role"] = Role;
    json["StartTime"] = StartTime;
    json["EndTime"] = EndTime;
    json["SessionDuration"] = TimeOverall;
    json["ScheduleOneDuration"] = TimeOne;
    json["ScheduleTwoDuration"] = TimeTwo;
    json["ScheduleThreeDuration"] = TimeThree;

    // Keys
    QJsonArray frequencyKeys;
    foreach(KeySetEntry entry, CurrentKeySet.FrequencyKeys)
    {
        QJsonObject mEntry;
        mEntry["Code"] = entry.KeyCode;
        mEntry["Name"] = entry.KeyName;
        mEntry["Description"] = entry.KeyDescription;

        frequencyKeys.append(mEntry);
    }
    json["FrequencyKeys"] = frequencyKeys;

    QJsonArray durationKeys;
    foreach(KeySetEntry entry, CurrentKeySet.DurationKeys)
    {
        QJsonObject mEntry;
        mEntry["Code"] = entry.KeyCode;
        mEntry["Name"] = entry.KeyName;
        mEntry["Description"] = entry.KeyDescription;

        durationKeys.append(mEntry);
    }
    json["DurationKeys"] = durationKeys;

    QJsonArray pressedKeys;
    foreach(SessionEvent event, *PressedKeys)
    {
        QJsonObject mEntry;
        mEntry["KeyCode"] = event.KeyEntered.KeyCode;
        mEntry["KeyName"] = event.KeyEntered.KeyName;
        mEntry["KeyDescription"] = event.KeyEntered.KeyDescription;
        mEntry["TimePressed"] = event.TimePressed.toString();
        mEntry["Schedule"] = formatSchedule(event.ScheduleType);
        mEntry["Measurement"] = formatMeasurement(event.MeasurementType);

        pressedKeys.append(mEntry);
    }
    json["PressedKeys"] = pressedKeys;

    QJsonArray frequencyOverall;
    for (int i(0); i<FrequencyKeysOverall->count(); i++)
    {
        QJsonObject mEntry;
        mEntry["Key"] = FrequencyKeysOverall->at(i).first;
        mEntry["Count"] = FrequencyKeysOverall->at(i).second;
        mEntry["Rate"] = formatRate(FrequencyKeysOverall->at(i).second, TimeOverall);
        mEntry["Measure"] = "Rate";

        frequencyOverall.append(mEntry);
    }
    json["FrequencyOverall"] = frequencyOverall;

    QJsonArray frequencyOne;
    for (int i(0); i<FrequencyKeysOne->count(); i++)
    {
        QJsonObject mEntry;
        mEntry["Key"] = FrequencyKeysOne->at(i).first;
        mEntry["Count"] = FrequencyKeysOne->at(i).second;
        mEntry["Rate"] = formatRate(FrequencyKeysOne->at(i).second, TimeOverall);
        mEntry["Measure"] = "Rate";

        frequencyOne.append(mEntry);
    }
    json["FrequencyOne"] = frequencyOne;

    QJsonArray frequencyTwo;
    for (int i(0); i<FrequencyKeysTwo->count(); i++)
    {
        QJsonObject mEntry;
        mEntry["Key"] = FrequencyKeysTwo->at(i).first;
        mEntry["Count"] = FrequencyKeysTwo->at(i).second;
        mEntry["Rate"] = formatRate(FrequencyKeysTwo->at(i).second, TimeOverall);
        mEntry["Measure"] = "Rate";

        frequencyTwo.append(mEntry);
    }
    json["FrequencyTwo"] = frequencyTwo;

    QJsonArray frequencyThree;
    for (int i(0); i<FrequencyKeysThree->count(); i++)
    {
        QJsonObject mEntry;
        mEntry["Key"] = FrequencyKeysThree->at(i).first;
        mEntry["Count"] = FrequencyKeysThree->at(i).second;
        mEntry["Rate"] = formatRate(FrequencyKeysThree->at(i).second, TimeOverall);
        mEntry["Measure"] = "Rate";

        frequencyThree.append(mEntry);
    }
    json["FrequencyThree"] = frequencyThree;


    QJsonArray durationOverall;
    for (int i(0); i<DurationKeysOverall->count(); i++)
    {
        QJsonObject mEntry;
        mEntry["Key"] = DurationKeysOverall->at(i).first;
        mEntry["Count"] = DurationKeysOverall->at(i).second;
        mEntry["Rate"] = formatPercentage(DurationKeysOverall->at(i).second, TimeOverall);
        mEntry["Measure"] = "Time";

        durationOverall.append(mEntry);
    }
    json["DurationOverall"] = durationOverall;

    QJsonArray durationOne;
    for (int i(0); i<DurationKeysOne->count(); i++)
    {
        QJsonObject mEntry;
        mEntry["Key"] = DurationKeysOne->at(i).first;
        mEntry["Count"] = DurationKeysOne->at(i).second;
        mEntry["Rate"] = formatPercentage(DurationKeysOne->at(i).second, TimeOne);
        mEntry["Measure"] = "Time";

        durationOne.append(mEntry);
    }
    json["DurationOne"] = durationOne;

    QJsonArray durationTwo;
    for (int i(0); i<DurationKeysTwo->count(); i++)
    {
        QJsonObject mEntry;
        mEntry["Key"] = DurationKeysTwo->at(i).first;
        mEntry["Count"] = DurationKeysTwo->at(i).second;
        mEntry["Rate"] = formatPercentage(DurationKeysTwo->at(i).second, TimeTwo);
        mEntry["Measure"] = "Time";

        durationTwo.append(mEntry);
    }
    json["DurationTwo"] = durationTwo;

    QJsonArray durationThree;
    for (int i(0); i<DurationKeysThree->count(); i++)
    {
        QJsonObject mEntry;
        mEntry["Key"] = DurationKeysThree->at(i).first;
        mEntry["Count"] = DurationKeysThree->at(i).second;
        mEntry["Rate"] = formatPercentage(DurationKeysThree->at(i).second, TimeThree);
        mEntry["Measure"] = "Time";

        durationThree.append(mEntry);
    }
    json["DurationThree"] = durationThree;

    QJsonDocument jsonDoc(json);

    QString mKeyPath = FileTools::pathAppend(mWorkingDirectory, Group);
    mKeyPath = FileTools::pathAppend(mKeyPath, Individual);
    mKeyPath = FileTools::pathAppend(mKeyPath, Evaluation);
    mKeyPath = FileTools::pathAppend(mKeyPath, Condition);

    QDir dir(mKeyPath);

    if (!dir.exists())
    {
        dir.mkpath(mKeyPath);
    }

    QString mFileName = QString("%1%2%3%4_%5.json")
            .arg(QString::number(CurrentKeySet.Session).rightJustified(3, '0'))
            .arg(Group.mid(0, 3))
            .arg(Individual.mid(0, 3))
            .arg(Evaluation.mid(0, 3))
            .arg(Role.mid(0, 1));

    QString path = FileTools::pathAppend(mKeyPath, mFileName);

    QFile saveFile(path);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        // TODO error handling
        return false;
    }

    if (saveFile.write(jsonDoc.toJson()) > -1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

/**
 * @brief WriteSessionSpreadsheet
 * @param mWorkingDirectory
 * @param CurrentKeySet
 * @param Group
 * @param Individual
 * @param Evaluation
 * @param Condition
 * @param Therapist
 * @param KeySetName
 * @param Collector
 * @param Role
 * @param StartTime
 * @param TimeOverall
 * @param TimeOne
 * @param TimeTwo
 * @param TimeThree
 * @param PressedKeys
 * @param FrequencyKeysOverall
 * @param DurationKeysOverall
 * @param FrequencyKeysOne
 * @param DurationKeysOne
 * @param FrequencyKeysTwo
 * @param DurationKeysTwo
 * @param FrequencyKeysThree
 * @param DurationKeysThree
 */
static bool WriteSessionSpreadsheet(QString mWorkingDirectory, KeySet CurrentKeySet, QString Group, QString Individual,
                                    QString Evaluation, QString Condition, QString Therapist,
                                    QString KeySetName, QString Collector, QString Role,
                                    QString StartTime, qint64 TimeOverall, qint64 TimeOne,
                                    qint64 TimeTwo, qint64 TimeThree, QList<SessionEvent> *PressedKeys,
                                    QList<QPair<QString,double>> * FrequencyKeysOverall, QList<QPair<QString,double>> * DurationKeysOverall,
                                    QList<QPair<QString,double>> * FrequencyKeysOne, QList<QPair<QString,double>> * DurationKeysOne,
                                    QList<QPair<QString,double>> * FrequencyKeysTwo, QList<QPair<QString,double>> * DurationKeysTwo,
                                    QList<QPair<QString,double>> * FrequencyKeysThree, QList<QPair<QString,double>> * DurationKeysThree)
{
    QXlsx::Document xlsx;

    // Cover Sheet

    xlsx.addSheet("Session Information");

    xlsx.write(1, 1, "Session Information");

    xlsx.write(3, 1, "Session");
    xlsx.write(3, 2, CurrentKeySet.Session);

    xlsx.write(4, 1, "Group");
    xlsx.write(4, 2, Group);

    xlsx.write(5, 1, "Individual");
    xlsx.write(5, 2, Individual);

    xlsx.write(6, 1, "Evaluation");
    xlsx.write(6, 2, Evaluation);

    xlsx.write(7, 1, "Condition");
    xlsx.write(7, 2, Condition);

    xlsx.write(8, 1, "Therapist");
    xlsx.write(8, 2, Therapist);

    xlsx.write(9, 1, "KeySet");
    xlsx.write(9, 2, KeySetName);

    xlsx.write(10,1, "Evaluation");
    xlsx.write(10,2, Evaluation);

    xlsx.write(11,1, "Collector");
    xlsx.write(11,2, Collector);

    xlsx.write(12,1, "Role");
    xlsx.write(12,2, Role);

    xlsx.write(14,1, "Start Time");
    xlsx.write(14,2, StartTime);

    xlsx.write(15,1, "End Time");
    xlsx.write(15,2, StartTime);

    xlsx.write(16,1, "Session Duration (seconds)");
    xlsx.write(16,2, (int)(TimeOverall / 1000));

    xlsx.write(17,1, "Schedule One Duration (seconds)");
    xlsx.write(17,2, (int)(TimeOne / 1000));

    xlsx.write(18,1, "Schedule Two Duration (seconds)");
    xlsx.write(18,2, (int)(TimeTwo / 1000));

    xlsx.write(19,1, "Schedule Three Duration (seconds)");
    xlsx.write(19,2, (int)(TimeThree / 1000));

    xlsx.write(21,1, "Frequency Keys");
    xlsx.write(21,2, "Frequency Counts");
    xlsx.write(21,3, "Frequency Rate/Minute");

    int row = 22;

    for (int i(0); i<FrequencyKeysOverall->count(); i++)
    {
        xlsx.write(row, 1, FrequencyKeysOverall->at(i).first);
        xlsx.write(row, 2, FrequencyKeysOverall->at(i).second);
        xlsx.write(row, 3, formatRate(FrequencyKeysOverall->at(i).second, TimeOverall));

        row++;
    }

    row++;

    xlsx.write(row, 1, "Duration Keys");
    xlsx.write(row, 2, "Duration Length");
    xlsx.write(row, 3, "Duration Percent");

    row++;

    for (int i(0); i<DurationKeysOverall->count(); i++)
    {
        xlsx.write(row, 1, DurationKeysOverall->at(i).first);
        xlsx.write(row, 2, DurationKeysOverall->at(i).second);
        xlsx.write(row, 3, formatPercentage(DurationKeysOverall->at(i).second, TimeOverall));

        row++;
    }

    // Schedule One

    xlsx.addSheet("Schedule One");

    xlsx.write(1,1, "Duration (seconds)");
    xlsx.write(1,2, (int)(TimeOne / 1000));

    xlsx.write(3,1, "Frequency Keys");
    xlsx.write(3,2, "Frequency Counts");
    xlsx.write(3,3, "Frequency Rate/Minute");

    row = 4;

    for (int i(0); i<FrequencyKeysOne->count(); i++)
    {
        xlsx.write(row, 1, FrequencyKeysOne->at(i).first);
        xlsx.write(row, 2, FrequencyKeysOne->at(i).second);
        xlsx.write(row, 3, formatRate(FrequencyKeysOne->at(i).second, TimeOne));

        row++;
    }

    row++;

    xlsx.write(row, 1, "Duration Keys");
    xlsx.write(row, 2, "Duration Counts");
    xlsx.write(row, 3, "Duration Percent");

    row++;

    for (int i(0); i<DurationKeysOne->count(); i++)
    {
        xlsx.write(row, 1, DurationKeysOne->at(i).first);
        xlsx.write(row, 2, DurationKeysOne->at(i).second);
        xlsx.write(row, 3, formatPercentage(DurationKeysOne->at(i).second, TimeOne));

        row++;
    }

    // Schedule Two

    xlsx.addSheet("Schedule Two");

    xlsx.write(1,1, "Duration (seconds)");
    xlsx.write(1,2, (int)(TimeTwo / 1000));

    xlsx.write(3,1, "Frequency Keys");
    xlsx.write(3,2, "Frequency Counts");
    xlsx.write(3,3, "Frequency Rate/Minute");

    row = 4;

    for (int i(0); i<FrequencyKeysTwo->count(); i++)
    {
        xlsx.write(row, 1, FrequencyKeysTwo->at(i).first);
        xlsx.write(row, 2, FrequencyKeysTwo->at(i).second);
        xlsx.write(row, 3, formatRate(FrequencyKeysTwo->at(i).second, TimeTwo));

        row++;
    }

    row++;

    xlsx.write(row, 1, "Duration Keys");
    xlsx.write(row, 2, "Duration Counts");
    xlsx.write(row, 3, "Duration Percent");

    row++;

    for (int i(0); i<DurationKeysOne->count(); i++)
    {
        xlsx.write(row, 1, DurationKeysTwo->at(i).first);
        xlsx.write(row, 2, DurationKeysTwo->at(i).second);
        xlsx.write(row, 3, formatPercentage(DurationKeysTwo->at(i).second, TimeTwo));

        row++;
    }

    // Schedule Three

    xlsx.addSheet("Schedule Three");

    xlsx.write(1,1, "Duration (seconds)");
    xlsx.write(1,2, (int)(TimeThree / 1000));

    xlsx.write(3,1, "Frequency Keys");
    xlsx.write(3,2, "Frequency Counts");
    xlsx.write(3,3, "Frequency Rate/Minute");

    row = 4;

    for (int i(0); i<FrequencyKeysThree->count(); i++)
    {
        xlsx.write(row, 1, FrequencyKeysThree->at(i).first);
        xlsx.write(row, 2, FrequencyKeysThree->at(i).second);
        xlsx.write(row, 3, formatRate(FrequencyKeysThree->at(i).second, TimeThree));

        row++;
    }

    row++;

    xlsx.write(row, 1, "Duration Keys");
    xlsx.write(row, 2, "Duration Counts");
    xlsx.write(row, 3, "Duration Percent");

    row++;

    for (int i(0); i<DurationKeysThree->count(); i++)
    {
        xlsx.write(row, 1, DurationKeysThree->at(i).first);
        xlsx.write(row, 2, DurationKeysThree->at(i).second);
        xlsx.write(row, 3, formatPercentage(DurationKeysThree->at(i).second, TimeThree));

        row++;
    }

    // Session Log (final sheet)

    xlsx.addSheet("Session Log");

    row = 1;

    xlsx.write(row, 1, "Coded Key");
    xlsx.write(row, 2, "Key Name");
    xlsx.write(row, 3, "Key Description");
    xlsx.write(row, 4, "Time Pressed");
    xlsx.write(row, 5, "Schedule");
    xlsx.write(row, 6, "Measurement Type");

    row++;

    foreach(SessionEvent event, *PressedKeys)
    {
        xlsx.write(row, 1, event.KeyEntered.KeyCode);
        xlsx.write(row, 2, event.KeyEntered.KeyName);
        xlsx.write(row, 3, event.KeyEntered.KeyDescription);
        xlsx.write(row, 4, event.TimePressed.toString());
        xlsx.write(row, 5, formatSchedule(event.ScheduleType));
        xlsx.write(row, 6, formatMeasurement(event.MeasurementType));

        row++;
    }

    QString mKeyPath = FileTools::pathAppend(mWorkingDirectory, Group);
    mKeyPath = FileTools::pathAppend(mKeyPath, Individual);
    mKeyPath = FileTools::pathAppend(mKeyPath, Evaluation);
    mKeyPath = FileTools::pathAppend(mKeyPath, Condition);

    QDir dir(mKeyPath);

    if (!dir.exists())
    {
        dir.mkpath(mKeyPath);
    }

    QString mFileName = QString("%1%2%3%4_%5.xlsx")
            .arg(QString::number(CurrentKeySet.Session).rightJustified(3, '0'))
            .arg(Group.mid(0, 3))
            .arg(Individual.mid(0, 3))
            .arg(Evaluation.mid(0, 3))
            .arg(Role.mid(0, 1));

    QString path = FileTools::pathAppend(mKeyPath, mFileName);

    return xlsx.saveAs(path);
}

static bool WriteReliSpreadsheet(QString mWorkingDirectory, QString Group, QString Individual, QString Evaluation, QList<ReliabilityMeasure> * ReliResults,
                                 QList<ReliabilityParse> * PrimaryReliabilityObjects, QList<ReliabilityParse> * SecondaryReliabilityObjects)
{
    QXlsx::Document xlsx;

    xlsx.addSheet("Session Information");

    ReliabilityMeasure temp;

    int spacer = 4;

    xlsx.write(1, 1, "Reliability Summary");
    xlsx.write(2, 1, "Group: " + Group);
    xlsx.write(3, 1, "Individual: " + Individual);
    xlsx.write(4, 1, "Group: " + Group);
    xlsx.write(5, 1, "Evaluation: " + Evaluation);
    xlsx.write(6, 1, "Processed: " + QDate::currentDate().toString());


    int topOffset = 9;

    xlsx.write(8, 1, "Session");
    xlsx.write(8, 2, "Primary");
    xlsx.write(8, 3, "Reliability");
    xlsx.write(8, 4, "Condition");

    for (int row(0); row < ReliResults->count(); row++)
    {
        temp = ReliResults->at(row);

        xlsx.write(topOffset + row, 1, temp.Session);
        xlsx.write(topOffset + row, 2, temp.Primary);
        xlsx.write(topOffset + row, 3, temp.Reliability);
        xlsx.write(topOffset + row, 4, temp.Condition);

        spacer = 5;

        if (row == 0) xlsx.write(topOffset - 3 + row, spacer, "Frequency Keys");

        for (int j(0); j<temp.fEIA.count(); j++)
        {
            if (row == 0) xlsx.write(topOffset - 2 + row, spacer, temp.fEIA.at(j).first);
            if (row == 0) xlsx.write(topOffset - 1 + row, spacer, "EIA");
            xlsx.write(topOffset + row, spacer, temp.fEIA.at(j).second);
            spacer++;

            if (row == 0) xlsx.write(topOffset - 1 + row, spacer, "PIA");
            xlsx.write(topOffset + row, spacer, temp.fPIA.at(j).second);
            spacer++;

            if (row == 0) xlsx.write(topOffset - 1 + row, spacer, "TIA");
            xlsx.write(topOffset + row, spacer, temp.fTIA.at(j).second);
            spacer++;

            if (row == 0) xlsx.write(topOffset - 1 + row, spacer, "OIA");
            xlsx.write(topOffset + row, spacer, temp.fOIA.at(j).second);
            spacer++;

            if (row == 0) xlsx.write(topOffset - 1 + row, spacer, "NIA");
            xlsx.write(topOffset + row, spacer, temp.fNIA.at(j).second);
            spacer++;

            if (row == 0) xlsx.write(topOffset - 1 + row, spacer, "PMA");
            xlsx.write(topOffset + row, spacer, temp.fPMA.at(j).second);
            spacer++;
        }

        if (row == 0) xlsx.write(topOffset - 3 + row, spacer, "Duration Keys");

        for (int j(0); j<temp.dEIA.count(); j++)
        {
            if (row == 0) xlsx.write(topOffset - 2 + row, spacer, temp.dEIA.at(j).first);
            if (row == 0) xlsx.write(topOffset - 1 + row, spacer, "EIA");
            xlsx.write(topOffset + row, spacer, temp.dEIA.at(j).second);
            spacer++;

            if (row == 0) xlsx.write(topOffset - 1 + row, spacer, "PIA");
            xlsx.write(topOffset + row, spacer, temp.dPIA.at(j).second);
            spacer++;

            if (row == 0) xlsx.write(topOffset - 1 + row, spacer, "TIA");
            xlsx.write(topOffset + row, spacer, temp.dTIA.at(j).second);
            spacer++;

            if (row == 0) xlsx.write(topOffset - 1 + row, spacer, "OIA");
            xlsx.write(topOffset + row, spacer, temp.dOIA.at(j).second);
            spacer++;

            if (row == 0) xlsx.write(topOffset - 1 + row, spacer, "NIA");
            xlsx.write(topOffset + row, spacer, temp.dNIA.at(j).second);
            spacer++;

            if (row == 0) xlsx.write(topOffset - 1 + row, spacer, "PMA");
            xlsx.write(topOffset + row, spacer, temp.dPMA.at(j).second);
            spacer++;
        }
    }


    int row = 10 + ReliResults->count();

    xlsx.write(row, 1, "Files Included:");

    row++;
    row++;

    for (int i(0); i<PrimaryReliabilityObjects->count(); i++)
    {
        xlsx.write(row, 1, PrimaryReliabilityObjects->at(i).PrimaryFilePath);

        row++;
    }

    for (int i(0); i<SecondaryReliabilityObjects->count(); i++)
    {
        xlsx.write(row, 1, SecondaryReliabilityObjects->at(i).PrimaryFilePath);

        row++;
    }

    QString mKeyPath = FileTools::pathAppend(mWorkingDirectory, Group);
    mKeyPath = FileTools::pathAppend(mKeyPath, Individual);
    mKeyPath = FileTools::pathAppend(mKeyPath, Evaluation);

    QDir dir(mKeyPath);

    if (!dir.exists())
    {
        dir.mkpath(mKeyPath);
    }

    QString mFileName = QString("Reli_%1%2%3.xlsx")
            .arg(Group.mid(0, 3))
            .arg(Individual.mid(0, 3))
            .arg(Evaluation.mid(0, 3));

    QString path = FileTools::pathAppend(mKeyPath, mFileName);

    return xlsx.saveAs(path);
}

/**
 * @brief formatSchedule
 * @param schedule
 * @return
 */
static QString formatSchedule(Schedule schedule)
{
    if (schedule == Schedule::One)
    {
        return QString("Schedule 1");
    }
    else if (schedule == Schedule::Two)
    {
        return QString("Schedule 2");
    }
    else
    {
        return QString("Schedule 3");
    }
}

/**
 * @brief formatMeasurement
 * @param measurement
 * @return
 */
static QString formatMeasurement(Measurement measurement)
{
    if (measurement == Measurement::Rate)
    {
        return QString("Rate");
    }
    else if (measurement == Measurement::Time)
    {
        return QString("Time");
    }
    else
    {
        return QString("Schedule");
    }
}

/** Format path for multi-platform use
 * @brief pathAppend
 * @param path
 * @param file
 * @return
 */
static QString pathAppend(const QString& path, const QString& file)
{
    return QDir::cleanPath(path + QDir::separator() + file);
}

/** Check and prepare a directory for writing
 * @brief CheckAndPrepDirectory
 * @param folderTitle
 * @return
 */
static bool CheckAndPrepDirectory(QString folderTitle)
{
    QStringList mDocFolder = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);

    if (mDocFolder.count() > 0)
    {
        QDir recoredDir(mDocFolder[0]);
        QFileInfoList mEntries = recoredDir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);

        if (!FileTools::CheckDirectory(mEntries, folderTitle))
        {
            recoredDir.mkdir(folderTitle);
        }

        if (FileTools::CheckDirectory(mEntries, folderTitle))
        {
            return true;
        }
    }

    return false;
}

/**
 * @brief CheckAndPrepDirectory
 * @param folderTitle
 * @param path
 * @return
 */
static bool CheckAndPrepDirectory(QString folderTitle, QString path)
{
    QDir mPath(path);

    if (mPath.exists())
    {
        QFileInfoList mEntries = mPath.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);

        if (!FileTools::CheckDirectory(mEntries, folderTitle))
        {
            mPath.mkdir(folderTitle);
        }

        if (FileTools::CheckDirectory(mEntries, folderTitle))
        {
            return true;
        }
    }

    return false;
}

/**
 * @brief CheckAndCreateFolder
 * @param folderTitle
 * @param path
 * @return
 */
static bool CheckAndCreateFolder(QString folderTitle, QString path)
{
    QDir mPath(path);

    if (mPath.exists())
    {
        QFileInfoList mEntries = mPath.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);

        if (!FileTools::CheckDirectory(mEntries, folderTitle))
        {
            return mPath.mkdir(folderTitle);
        }
        else
        {
            return true;
        }
    }

    return false;
}

/** Check for designated folder
 * @brief CheckDirectory
 * @param mEntries
 * @param folderTitle
 * @return
 */
static bool CheckDirectory(QFileInfoList mEntries, QString folderTitle)
{
    for (QFileInfo mInf : mEntries)
    {
        if (mInf.isDir())
        {
            if (mInf.fileName() == folderTitle)
            {
                return true;
            }
        }
    }

    return false;
}

};

#endif // FILETOOLS_H
