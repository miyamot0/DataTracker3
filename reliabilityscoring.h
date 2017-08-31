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

#ifndef RELIABILITYSCORING_H
#define RELIABILITYSCORING_H

#include <QObject>
#include <QDir>

#include "keysetentry.h"
#include "sessionevent.h"
#include "reliabilitymeasure.h"
#include "reliabilityparse.h"
#include "filetools.h"

class ReliabilityScoring
{
public:

static bool PerformReliabilityCheck(QString mWorkingDirectory, QString Group, QString Individual, QString Evaluation, QList<QStringList> * outputDisplay)
{
    QString mFilePath = FileTools::pathAppend(mWorkingDirectory, Group);
    mFilePath = FileTools::pathAppend(mFilePath, Individual);
    mFilePath = FileTools::pathAppend(mFilePath, Evaluation);

    QList<ReliabilityParse> PrimaryReliabilityObjects;
    QList<ReliabilityParse> SecondaryReliabilityObjects;

    QList<ReliabilityMeasure> ReliResults;

    QDirIterator iterator(mFilePath,
                          QStringList() << "*.json",
                          QDir::Files,
                          QDirIterator::Subdirectories);

    while (iterator.hasNext())
    {
        QString mFileName = iterator.next();

        if (mFileName.contains(".json", Qt::CaseInsensitive))
        {
            QFile mSession(mFileName);

            if (mSession.exists())
            {
                if (mSession.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    QString sessionData = mSession.readAll();
                    mSession.close();

                    QJsonDocument loadSession = QJsonDocument::fromJson(sessionData.toUtf8());

                    if (!loadSession.isNull())
                    {
                        QJsonObject sessionObject = loadSession.object();

                        if((sessionObject["Role"].toString().contains("Primary", Qt::CaseInsensitive)))
                        {
                            ReliabilityParse mReliObj;
                            mReliObj.SessionNumber = sessionObject["Session"].toInt();
                            mReliObj.Collector = sessionObject["Collector"].toString();
                            mReliObj.Condition = sessionObject["Condition"].toString();
                            mReliObj.SecondaryObserver = QString("---");
                            mReliObj.PrimaryFilePath = mFileName;
                            mReliObj.Reli = false;
                            mReliObj.CanScoreAsReli = false;

                            PrimaryReliabilityObjects.append(mReliObj);
                        }
                        else
                        {
                            ReliabilityParse mReliObj;
                            mReliObj.SessionNumber = sessionObject["Session"].toInt();
                            mReliObj.Collector = sessionObject["Collector"].toString();
                            mReliObj.Condition = sessionObject["Condition"].toString();
                            mReliObj.PrimaryFilePath = mFileName;
                            mReliObj.Reli = true;
                            mReliObj.CanScoreAsReli = false;

                            SecondaryReliabilityObjects.append(mReliObj);
                        }
                    }
                }
            }
        }
    }

    for (int i(0); i < PrimaryReliabilityObjects.count(); i++)
    {
        for (int j(0); j < SecondaryReliabilityObjects.count(); j++)
        {
            if (PrimaryReliabilityObjects[i].SessionNumber == SecondaryReliabilityObjects[j].SessionNumber &&
                    PrimaryReliabilityObjects[i].Condition == SecondaryReliabilityObjects[j].Condition)
            {
                PrimaryReliabilityObjects[i].SecondaryObserver = SecondaryReliabilityObjects[j].Collector;
                PrimaryReliabilityObjects[i].ReliFilePath = SecondaryReliabilityObjects[j].PrimaryFilePath;
                PrimaryReliabilityObjects[i].CanScoreAsReli = true;
            }
        }
    }

    QJsonObject mPrimary, mReli;

    bool mPrimaryCheck = false,
         mReliCheck = false;

    QDateTime startTime, endTime;

    ReliResults.clear();

    for(int i(0); i<PrimaryReliabilityObjects.count(); i++)
    {
        if (PrimaryReliabilityObjects[i].CanScoreAsReli)
        {
            ReliabilityMeasure mMeasure;

            mMeasure.Session = PrimaryReliabilityObjects[i].SessionNumber;
            mMeasure.Group = Group;
            mMeasure.Individual = Individual;
            mMeasure.Evaluation = Evaluation;
            mMeasure.Condition = PrimaryReliabilityObjects[i].Condition;
            mMeasure.Primary = PrimaryReliabilityObjects[i].Collector;
            mMeasure.Reliability = PrimaryReliabilityObjects[i].SecondaryObserver;

            mPrimaryCheck = FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects[i].PrimaryFilePath, &mPrimary);
            mReliCheck = FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects[i].ReliFilePath, &mReli);

            if (mPrimaryCheck && mReliCheck)
            {
                ReliabilityScoring::CompareObservers(mPrimary, mReli, &mMeasure);
            }

            ReliResults.append(mMeasure);
        }
    }

    bool success = FileTools::WriteReliSpreadsheet(mWorkingDirectory,
                                                   Group,
                                                   Individual,
                                                   Evaluation,
                                                   &ReliResults,
                                                   &PrimaryReliabilityObjects,
                                                   &SecondaryReliabilityObjects,
                                                   outputDisplay);

    return success;
}

/**
 * @brief CompareObservers
 * @param mPrimary
 * @param mSecondary
 * @param mMeasure
 */
static void CompareObservers(QJsonObject mPrimary, QJsonObject mSecondary, ReliabilityMeasure * mMeasure)
{
    QDateTime startTime = QDateTime(QDateTime::fromString(mPrimary["StartTime"].toString()));
    QDateTime endTime = QDateTime(QDateTime::fromString(mPrimary["EndTime"].toString()));

    int totalSecs = (int)((double) mPrimary["SessionDuration"].toInt() / 1000);

    int bins = totalSecs / 10;
    int overflow = ((totalSecs % 10) > 0) ? 1 : 0;
        bins = bins + overflow;

    QList<KeySetEntry> PrimaryFrequencyKeys;

    QJsonArray frequencyArray = mPrimary["FrequencyKeys"].toArray();
    foreach (const QJsonValue collector, frequencyArray) {
        QJsonObject mObj = collector.toObject();

        KeySetEntry mEntry;
        mEntry.KeyCode = mObj["Code"].toInt();
        mEntry.KeyDescription = mObj["Description"].toString();
        mEntry.KeyName = mObj["Name"].toString();

        PrimaryFrequencyKeys.append(mEntry);
    }

    QList<KeySetEntry> PrimaryDurationKeys;

    QJsonArray durationArray = mPrimary["DurationKeys"].toArray();
    foreach (const QJsonValue collector, durationArray) {
        QJsonObject mObj = collector.toObject();

        KeySetEntry mEntry;
        mEntry.KeyCode = mObj["Code"].toInt();
        mEntry.KeyDescription = mObj["Description"].toString();
        mEntry.KeyName = mObj["Name"].toString();

        PrimaryDurationKeys.append(mEntry);
    }

    QList<SessionEvent> PrimaryPressedKeys;

    QJsonArray pressedKeysJson = mPrimary["PressedKeys"].toArray();
    foreach (const QJsonValue collector, pressedKeysJson) {
        QJsonObject mObj = collector.toObject();

        SessionEvent mEntry;
        mEntry.KeyEntered.KeyCode = mObj["KeyCode"].toInt();
        mEntry.KeyEntered.KeyDescription = mObj["KeyDescription"].toString();
        mEntry.KeyEntered.KeyName = mObj["KeyName"].toString();
        //mEntry.MeasurementType = mObj["Measurement"].toString();
        //mEntry.ScheduleType = mObj["Schedule"].toString();
        mEntry.TimePressed = QDateTime(QDateTime::fromString(mObj["TimePressed"].toString()));

        PrimaryPressedKeys.append(mEntry);
    }

    // Reli
    QList<KeySetEntry> ReliFrequencyKeys;

    frequencyArray = mSecondary["FrequencyKeys"].toArray();
    foreach (const QJsonValue collector, frequencyArray) {
        QJsonObject mObj = collector.toObject();

        KeySetEntry mEntry;
        mEntry.KeyCode = mObj["Code"].toInt();
        mEntry.KeyDescription = mObj["Description"].toString();
        mEntry.KeyName = mObj["Name"].toString();

        ReliFrequencyKeys.append(mEntry);
    }

    QList<KeySetEntry> ReliDurationKeys;

    durationArray = mSecondary["DurationKeys"].toArray();
    foreach (const QJsonValue collector, durationArray) {
        QJsonObject mObj = collector.toObject();

        KeySetEntry mEntry;
        mEntry.KeyCode = mObj["Code"].toInt();
        mEntry.KeyDescription = mObj["Description"].toString();
        mEntry.KeyName = mObj["Name"].toString();

        ReliDurationKeys.append(mEntry);
    }

    QList<SessionEvent> ReliPressedKeys;

    pressedKeysJson = mSecondary["PressedKeys"].toArray();
    foreach (const QJsonValue collector, pressedKeysJson) {
        QJsonObject mObj = collector.toObject();

        SessionEvent mEntry;
        mEntry.KeyEntered.KeyCode = mObj["KeyCode"].toInt();
        mEntry.KeyEntered.KeyDescription = mObj["KeyDescription"].toString();
        mEntry.KeyEntered.KeyName = mObj["KeyName"].toString();
        //mEntry.MeasurementType = mObj["Measurement"].toString();
        //mEntry.ScheduleType = mObj["Schedule"].toString();
        mEntry.TimePressed = QDateTime(QDateTime::fromString(mObj["TimePressed"].toString()));

        ReliPressedKeys.append(mEntry);
    }

    QList<KeySetEntry> SharedParseFrequencyKeySet;
    for (int i(0); i<PrimaryFrequencyKeys.count(); i++)
    {
        for (int j(0); j<ReliFrequencyKeys.count(); j++)
        {
            if (PrimaryFrequencyKeys[i].KeyCode == ReliFrequencyKeys[i].KeyCode)
            {
                SharedParseFrequencyKeySet.append(PrimaryFrequencyKeys[i]);
            }
        }
    }

    QList<KeySetEntry> SharedParseDurationKeySet;
    for (int i(0); i<PrimaryDurationKeys.count(); i++)
    {
        for (int j(0); j<ReliDurationKeys.count(); j++)
        {
            if (PrimaryDurationKeys[i].KeyCode == ReliDurationKeys[i].KeyCode)
            {
                SharedParseDurationKeySet.append(PrimaryDurationKeys[i]);
            }
        }
    }

    // Keys sorted

    QList<QList<int>> mPrimaryFrequencyBins = ReliabilityScoring::GetFrequencyBins(bins, SharedParseFrequencyKeySet, startTime, PrimaryPressedKeys);
    QList<QList<double>> mPrimaryDurationBins = ReliabilityScoring::GetDurationBins(bins, SharedParseDurationKeySet, startTime, endTime, PrimaryPressedKeys);

    QList<QList<int>> mSecondaryFrequencyBins   = ReliabilityScoring::GetFrequencyBins(bins, SharedParseFrequencyKeySet, startTime, ReliPressedKeys);
    QList<QList<double>> mSecondaryDurationBins = ReliabilityScoring::GetDurationBins(bins, SharedParseDurationKeySet, startTime, endTime, ReliPressedKeys);

    for (int i(0); i<PrimaryFrequencyKeys.count(); i++)
    {
        mMeasure->fEIA.append(QPair<QString,QString>(PrimaryFrequencyKeys[i].KeyName, getFrequencyEIA(&mPrimaryFrequencyBins[i], &mSecondaryFrequencyBins[i])));
        mMeasure->fPIA.append(QPair<QString,QString>(PrimaryFrequencyKeys[i].KeyName, getFrequencyPIA(&mPrimaryFrequencyBins[i], &mSecondaryFrequencyBins[i])));
        mMeasure->fTIA.append(QPair<QString,QString>(PrimaryFrequencyKeys[i].KeyName, getFrequencyTIA(&mPrimaryFrequencyBins[i], &mSecondaryFrequencyBins[i])));
        mMeasure->fOIA.append(QPair<QString,QString>(PrimaryFrequencyKeys[i].KeyName, getFrequencyOIA(&mPrimaryFrequencyBins[i], &mSecondaryFrequencyBins[i])));
        mMeasure->fNIA.append(QPair<QString,QString>(PrimaryFrequencyKeys[i].KeyName, getFrequencyNIA(&mPrimaryFrequencyBins[i], &mSecondaryFrequencyBins[i])));
        mMeasure->fPMA.append(QPair<QString,QString>(PrimaryFrequencyKeys[i].KeyName, getFrequencyPMA(&mPrimaryFrequencyBins[i], &mSecondaryFrequencyBins[i])));
    }

    for (int i(0); i<PrimaryDurationKeys.count(); i++)
    {
        mMeasure->dEIA.append(QPair<QString,QString>(PrimaryFrequencyKeys[i].KeyName, getDurationEIA(mPrimaryDurationBins[i], mSecondaryDurationBins[i])));
        mMeasure->dPIA.append(QPair<QString,QString>(PrimaryFrequencyKeys[i].KeyName, getDurationPIA(mPrimaryDurationBins[i], mSecondaryDurationBins[i])));
        mMeasure->dTIA.append(QPair<QString,QString>(PrimaryFrequencyKeys[i].KeyName, getDurationTIA(mPrimaryDurationBins[i], mSecondaryDurationBins[i])));
        mMeasure->dOIA.append(QPair<QString,QString>(PrimaryFrequencyKeys[i].KeyName, getDurationOIA(mPrimaryDurationBins[i], mSecondaryDurationBins[i])));
        mMeasure->dNIA.append(QPair<QString,QString>(PrimaryFrequencyKeys[i].KeyName, getDurationNIA(mPrimaryDurationBins[i], mSecondaryDurationBins[i])));
        mMeasure->dPMA.append(QPair<QString,QString>(PrimaryFrequencyKeys[i].KeyName, getDurationPMA(mPrimaryDurationBins[i], mSecondaryDurationBins[i])));
    }
}

/**
 * @brief GetFrequencyBins
 * @param bins
 * @param FrequencyKeys
 * @param startTime
 * @param PressedKeys
 * @return
 */
static QList<QList<int>> GetFrequencyBins(int bins, QList<KeySetEntry> FrequencyKeys, QDateTime startTime, QList<SessionEvent> PressedKeys)
{
    QList<QList<int>> mFrequencyBins;
    int fKeys = FrequencyKeys.count();

    QList<int> tempList;

    for (int i(0); i<fKeys; i++)
    {
        tempList.clear();

        for (int j(0); j<bins; j++)
        {
            tempList.append(0);
        }

        mFrequencyBins.append(tempList);
    }

    KeySetEntry temp;
    int timeHolder;

    for (int i(0); i<fKeys; i++)
    {
        temp = FrequencyKeys.at(i);

        foreach(SessionEvent event, PressedKeys)
        {
            if (event.KeyEntered.KeyCode == temp.KeyCode)
            {
                timeHolder = startTime.secsTo(event.TimePressed) / 10;
                mFrequencyBins[i][timeHolder] = mFrequencyBins[i][timeHolder] + 1;
            }
        }
    }

    return mFrequencyBins;
}

/**
 * @brief GetDurationBins
 * @param bins
 * @param DurationKeys
 * @param startTime
 * @param endTime
 * @param PressedKeys
 * @return
 */
static QList<QList<double>> GetDurationBins(int bins, QList<KeySetEntry> DurationKeys, QDateTime startTime, QDateTime endTime, QList<SessionEvent> PressedKeys)
{
    QList<QList<double>> mDurationBins;
    int dKeys = DurationKeys.count();

    QList<double> tempList;

    for (int i(0); i<dKeys; i++)
    {
        tempList.clear();

        for (int j(0); j<bins; j++)
        {
            tempList.append(0);
        }

        mDurationBins.append(tempList);
    }

    bool waitingForNext = false;
    QDateTime prev, after;

    QList<QPair<qint64, qint64>> runs;
    qint64 start, stop;

    KeySetEntry temp;

    for (int i(0); i<dKeys; i++)
    {
        temp = DurationKeys.at(i);
        waitingForNext = false;
        runs.clear();

        foreach(SessionEvent event, PressedKeys)
        {
            if (event.KeyEntered.KeyCode == temp.KeyCode)
            {
                if (waitingForNext)
                {
                    after = event.TimePressed;
                    runs.append(QPair<qint64, qint64>(startTime.msecsTo(prev), startTime.msecsTo(after)));
                    waitingForNext = false;
                }
                else
                {
                    prev = event.TimePressed;
                    waitingForNext = true;
                }
            }
        }

        if (waitingForNext)
        {
            runs.append(QPair<qint64, qint64>(startTime.msecsTo(prev), (startTime.msecsTo(endTime) - (startTime.msecsTo(endTime) % 1000))));
        }

        for (int j(0); j<runs.length(); j++)
        {
            start = runs[j].first;
            stop = runs[j].second;

            int index1 = (int)(start / 10000);
            int index2 = (int)(stop / 10000);

            // !important In case overshot
            if (index2 == bins)
            {
                index2--;
            }

            if (index1 == index2)
            {
                mDurationBins[i][index1] = mDurationBins[i][index1] + ((double)(stop - start))/1000;
            }
            else if ((index2 - index1) == 1)
            {
                mDurationBins[i][index1] = mDurationBins[i][index1] + ((double)(10000 - (start % 10000)))/1000;
                mDurationBins[i][index2] = mDurationBins[i][index2] + ((double)(stop % 10000))/1000;
            }
            else
            {
                mDurationBins[i][index1] = mDurationBins[i][index1] + ((double)(10000 - (start % 10000)))/1000;

                for (int k(index1 + 1); k < index2; k++)
                {
                    mDurationBins[i][k] = mDurationBins[i][k] + 10;
                }

                mDurationBins[i][index2] = mDurationBins[i][index2] + ((double)(stop % 10000))/1000;
            }
        }
    }

    return mDurationBins;
}

/**
 * @brief GetLengthCompare
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static int GetLengthCompare(QList<double> * mPrimary, QList<double> * mSecondary)
{
    return (mPrimary->count() == mSecondary->count() ?
                mPrimary->count() : (mPrimary->count() > mSecondary->count()) ?
                    mSecondary->count() : mPrimary->count());
}

/**
 * @brief GetLengthCompare
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static int GetLengthCompare(QList<int> * mPrimary, QList<int> * mSecondary)
{
    return (mPrimary->count() == mSecondary->count() ?
                mPrimary->count() : (mPrimary->count() > mSecondary->count()) ?
                    mSecondary->count() : mPrimary->count());
}

/**
 * @brief formatResult
 * @param sum
 * @param count
 * @return
 */
static QString formatResult(double sum, double count)
{
    if (count == 0)
    {
        return QString("NaN");
    }
    else
    {
        return QString::number(((sum / count) * 100));
    }
}

/**
 * @brief getFrequenyEIA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static QString getFrequencyEIA(QList<int> * mPrimary, QList<int> * mSecondary)
{
    if ((mPrimary->count() == 0))
    {
        return QString("NaN");
    }

    if ((mSecondary->count() == 0))
    {
        return QString("NaN");
    }

    int runLength = GetLengthCompare(mPrimary, mSecondary);

    int count = 0;
    int sum = 0;

    for (int i(0); i<runLength; i++)
    {
        if (mPrimary[i] == mSecondary[i])
        {
            sum ++;
        }
        count++;
    }

    return formatResult((double) sum, (double) count);
    //return ((double) sum / (double) count) * 100;
}

/**
 * @brief getDurationEIA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static QString getDurationEIA(QList<double> mPrimary, QList<double> mSecondary)
{
    if (mPrimary.count() == 0)
    {
        return QString("NaN");
    }

    if (mSecondary.count() == 0)
    {
        return QString("NaN");
    }

    int runLength = GetLengthCompare(&mPrimary, &mSecondary);

    int count = 0;
    int sum = 0;

    for (int i(0); i<runLength; i++)
    {
        if (mPrimary[i] == mSecondary[i])
        {
            sum ++;
        }
        count++;
    }

    return formatResult((double) sum, (double) count);
}

/**
 * @brief getFreqPIA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static QString getFrequencyPIA(QList<int> * mPrimary, QList<int> * mSecondary)
{
    if ((mPrimary->count() == 0))
    {
        return QString("NaN");
    }

    if ((mSecondary->count() == 0))
    {
        return QString("NaN");
    }

    int runLength = GetLengthCompare(mPrimary, mSecondary);

    double higher, lower;

    int count = 0;
    double sum = 0.0;

    for (int i(0); i < runLength; i++)
    {
        if (mPrimary[i] == mSecondary[i])
        {
            sum += 1.0;
        }
        else
        {
            higher = (mPrimary[i] > mSecondary[i]) ? mPrimary->at(i) : mSecondary->at(i);
            lower = (mPrimary[i] > mSecondary[i]) ? mSecondary->at(i) : mPrimary->at(i);

            sum += (lower / higher);
        }

        count++;
    }

    return formatResult(sum, (double) count);
    //return ((double) sum / (double) count) * 100;
}

/**
 * @brief getDurationPIA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static QString getDurationPIA(QList<double> mPrimary, QList<double> mSecondary)
{
    if ((mPrimary.count() == 0))
    {
        return QString("NaN");
    }

    if ((mSecondary.count() == 0))
    {
        return QString("NaN");
    }

    int runLength = GetLengthCompare(&mPrimary, &mSecondary);

    double higher, lower;

    int count = 0;
    double sum = 0.0;

    for (int i(0); i < runLength; i++)
    {
        if (qRound(mPrimary[i]) == qRound(mSecondary[i]))
        {
            sum += 1.0;
        }
        else
        {
            higher = (mPrimary[i] > mSecondary[i]) ? mPrimary[i] : mSecondary[i];
            lower = (mPrimary[i] > mSecondary[i]) ? mSecondary[i] : mPrimary[i];

            sum += (lower / higher);
        }

        count++;
    }

    return formatResult(sum, (double) count);
}

/**
 * @brief getFreqTIA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static QString getFrequencyTIA(QList<int> * mPrimary, QList<int> * mSecondary)
{
    if ((mPrimary->count() == 0))
    {
        return QString("NaN");
    }

    if ((mSecondary->count() == 0))
    {
        return QString("NaN");
    }

    int runLength = GetLengthCompare(mPrimary, mSecondary);

    int count = 0;
    double sum = 0.0;

    for (int i(0); i < runLength; i++)
    {
        if (mPrimary->at(i) == 0 && mSecondary->at(i) == 0)
        {
            sum += 1.0;
        }
        else if (mPrimary->at(i) > 0 && mSecondary->at(i) > 0)
        {
            sum += 1.0;
        }

        count++;
    }

    return formatResult(sum, (double) count);
    //return ((double) sum / (double) count) * 100;
}

/**
 * @brief getDurationTIA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static QString getDurationTIA(QList<double> mPrimary, QList<double> mSecondary)
{
    if ((mPrimary.count() == 0))
    {
        return QString("NaN");
    }

    if ((mSecondary.count() == 0))
    {
        return QString("NaN");
    }

    int runLength = GetLengthCompare(&mPrimary, &mSecondary);

    int count = 0;
    double sum = 0.0;

    for (int i(0); i < runLength; i++)
    {
        if (qRound(mPrimary[i]) == 0 && qRound(mSecondary[i]) == 0)
        {
            sum += 1.0;
        }
        else if (mPrimary[i] > 0 && mSecondary[i] > 0)
        {
            sum += 1.0;
        }

        count++;
    }

    return formatResult(sum, (double) count);
}

/**
 * @brief getFreqOIA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static QString getFrequencyOIA(QList<int> * mPrimary, QList<int> * mSecondary)
{
    if ((mPrimary->count() == 0))
    {
        return QString("NaN");
    }

    if ((mSecondary->count() == 0))
    {
        return QString("NaN");
    }

    int runLength = GetLengthCompare(mPrimary, mSecondary);

    int count = 0;
    double sum = 0.0;

    for (int i(0); i < runLength; i++)
    {
        if (mPrimary->at(i) > 0 || mSecondary->at(i) > 0)
        {
            if (mPrimary->at(i) == 0 && mSecondary->at(i) == 0)
            {
                sum += 1.0;
            }
            else if (mPrimary->at(i) > 0 && mSecondary->at(i) > 0)
            {
                sum += 1.0;
            }
            count++;
        }
    }

    return formatResult(sum, (double) count);
    //return ((double) sum / (double) count) * 100;
}

/**
 * @brief getDurationOIA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static QString getDurationOIA(QList<double> mPrimary, QList<double> mSecondary)
{
    if ((mPrimary.count() == 0))
    {
        return QString("NaN");
    }

    if ((mSecondary.count() == 0))
    {
        return QString("NaN");
    }

    int runLength = GetLengthCompare(&mPrimary, &mSecondary);

    int count = 0;
    double sum = 0.0;

    for (int i(0); i < runLength; i++)
    {
        if (mPrimary[i] > 0 || mSecondary[i] > 0)
        {
            if (qRound(mPrimary[i]) == 0 && qRound(mSecondary[i]) == 0)
            {
                sum += 1.0;
            }
            else if (mPrimary[i] > 0 && mSecondary[i] > 0)
            {
                sum += 1.0;
            }

            count++;
        }
    }

    return formatResult(sum, (double) count);
}

/**
 * @brief getFreqNIA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static QString getFrequencyNIA(QList<int> * mPrimary, QList<int> * mSecondary)
{
    if ((mPrimary->count() == 0))
    {
        return QString("NaN");
    }

    if ((mSecondary->count() == 0))
    {
        return QString("NaN");
    }

    int runLength = GetLengthCompare(mPrimary, mSecondary);

    int count = 0;
    double sum = 0.0;

    for (int i(0); i < runLength; i++)
    {
        if (mPrimary->at(i) < 1 || mSecondary->at(i) < 1)
        {
            if (mPrimary->at(i) == 0 && mSecondary->at(i) == 0)
            {
                sum += 1.0;
            }
            else if (mPrimary->at(i) > 0 && mSecondary->at(i) > 0)
            {
                sum += 1.0;
            }
            count++;
        }
    }

    return formatResult(sum, (double) count);
    //return ((double) sum / (double) count) * 100;
}

/**
 * @brief getDurationNIA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static QString getDurationNIA(QList<double> mPrimary, QList<double> mSecondary)
{
    if ((mPrimary.count() == 0))
    {
        return QString("NaN");
    }

    if ((mSecondary.count() == 0))
    {
        return QString("NaN");
    }

    int runLength = GetLengthCompare(&mPrimary, &mSecondary);

    int count = 0;
    double sum = 0.0;

    for (int i(0); i < runLength; i++)
    {
        if (mPrimary[i] < 1 || mSecondary[i] < 1)
        {
            if (qRound(mPrimary[i]) == 0 && qRound(mSecondary[i]) == 0)
            {
                sum += 1.0;
            }
            else if (mPrimary[i] > 0 && mSecondary[i] > 0)
            {
                sum += 1.0;
            }

            count++;
        }
    }

    return formatResult(sum, (double) count);
}

/**
 * @brief getFreqPMA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static QString getFrequencyPMA(QList<int> * mPrimary, QList<int> * mSecondary)
{
    if ((mPrimary->count() == 0))
    {
        return QString("NaN");
    }

    if ((mSecondary->count() == 0))
    {
        return QString("NaN");
    }

    int runLength = GetLengthCompare(mPrimary, mSecondary);

    int count = 0,
        innerCount = 0;

    double sum = 0.0,
           innerPrim = 0.0,
           innerReli = 0.0,
           higher,
           lower;

    for (int i(0); i < runLength; i++)
    {
        innerPrim += mPrimary->at(i);
        innerReli += mSecondary->at(i);

        innerCount++;

        if ((innerCount+1) % 6 == 0)
        {
            if (innerPrim == innerReli)
            {
                sum += 1.0;
            }
            else
            {
                higher = (innerPrim > innerReli) ? innerPrim : innerReli;
                lower = (innerPrim > innerReli) ? innerReli : innerPrim;

                sum += (lower / higher);
            }

            innerPrim = innerReli = 0;

            count++;
        }
    }

    return formatResult(sum, (double) count);
}

/**
 * @brief getDurationPMA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static QString getDurationPMA(QList<double> mPrimary, QList<double> mSecondary)
{
    if ((mPrimary.count() == 0))
    {
        return QString("NaN");
    }

    if ((mSecondary.count() == 0))
    {
        return QString("NaN");
    }

    int runLength = GetLengthCompare(&mPrimary, &mSecondary);

    int count = 0,
        innerCount = 0;

    double sum = 0.0,
           innerPrim = 0.0,
           innerReli = 0.0,
           higher,
           lower;

    for (int i(0); i < runLength; i++)
    {
        innerPrim += qRound(mPrimary[i]);
        innerReli += qRound(mSecondary[i]);

        innerCount++;

        if ((innerCount + 1) % 6 == 0)
        {
            if (innerPrim == innerReli)
            {
                sum += 1.0;
            }
            else
            {
                higher = (innerPrim > innerReli) ? innerPrim : innerReli;
                lower = (innerPrim > innerReli) ? innerReli : innerPrim;


                sum += (lower / higher);
            }


            innerPrim = innerReli = 0;
            count++;
        }
    }

    return formatResult(sum, (double) count);
}


};

#endif // RELIABILITYSCORING_H
