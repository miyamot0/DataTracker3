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

#include "directoryparse.h"
#include "parsetypes.h"
#include "keyset.h"
#include "keysetentry.h"
#include "sessionevent.h"

class FileTools
{
public:

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
static void WriteSessionJSON(QString mWorkingDirectory, KeySet CurrentKeySet, QString Group, QString Individual,
                             QString Evaluation, QString Condition, QString Therapist,
                             QString KeySetName, QString Collector, QString Role,
                             QString StartTime, qint64 TimeOverall, qint64 TimeOne,
                             qint64 TimeTwo, qint64 TimeThree, QList<SessionEvent> *PressedKeys)
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
        return;
    }

    saveFile.write(jsonDoc.toJson());
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
