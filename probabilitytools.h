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

#ifndef PROBABILITYTOOLS_H
#define PROBABILITYTOOLS_H

#include <QFile>
#include <QObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "reliabilityparse.h"
#include "lagcoding.h"

#include <QDebug>

class ProbabilityTools
{
public:

enum ProbabilityType
{
    OddsRatio,
    YulesQ,
    OperantContingencyValue,
    Blank
};

/**
 * @brief FillMetaContingencyTable
 * @param PrimaryReliabilityObjects
 * @param mResults
 * @param keyList
 * @param mScoreKey
 * @param tableConstruction
 * @param binSize
 * @param windowSize
 */
static void FillMetaContingencyTable(QList<ReliabilityParse> * PrimaryReliabilityObjects,
                                     QList<QStringList> * mResults,
                                     QStringList keyList,
                                     QList<QPair<QString, int>> mScoreKey,
                                     QList<QList<QPair<LagCoding, LagCoding>>> tableConstruction,
                                     ProbabilityType analysisType,
                                     int binSize = 1,
                                     int windowSize = 4)
{
    QJsonObject json;

    int sessionDuration;
    int secs;
    int binCount;

    QStringList temp;

    QList<QList<int>> scoringBins;
    QDateTime startTime, endTime;

    //Loop
    for (int i(0); i<PrimaryReliabilityObjects->count(); i++)
    {
        // each is a session here
        QFile mSession(PrimaryReliabilityObjects->at(i).PrimaryFilePath);

        if (mSession.exists())
        {
            if (mSession.open(QIODevice::ReadOnly | QIODevice::Text))
            {

                QString sessionData = mSession.readAll();
                mSession.close();

                QJsonDocument loadSession = QJsonDocument::fromJson(sessionData.toUtf8());
                json = loadSession.object();

                sessionDuration = (json["SessionDuration"].toInt() / 1000);
                startTime = QDateTime(QDateTime::fromString(json["StartTime"].toString()));
                binCount = sessionDuration / binSize;

                scoringBins.clear();

                for (int i(0); i< keyList.count(); i++)
                {
                    QList<int> temp;

                    for (int j(0); j < binCount; j++)
                    {
                        temp << 0;
                    }

                    scoringBins.append(temp);
                }

                // Frequencies here
                QJsonArray pressedKeysJson = json["PressedKeys"].toArray();
                foreach (const QJsonValue keyObj, pressedKeysJson) {
                    QJsonObject mObj = keyObj.toObject();

                    int fIndex = keyList.indexOf(mObj["KeyDescription"].toString());

                    if (fIndex != -1 && mScoreKey[fIndex].second == 1)
                    {
                        secs = startTime.secsTo(QDateTime::fromString(mObj["TimePressed"].toString()));
                        int locationInArray = secs / binSize;

                        scoringBins[fIndex][locationInArray] = 1;
                    }
                }

                // durations here
                bool waitingForNext = false;
                int prev, after;

                for (int i(0); i<mScoreKey.count(); i++)
                {
                    // Is a duration key
                    if (mScoreKey.at(i).second == 2)
                    {
                        foreach (const QJsonValue keyObj, pressedKeysJson) {
                            QJsonObject mObj = keyObj.toObject();

                            if (mObj["KeyDescription"].toString() == mScoreKey.at(i).first)
                            {
                                if (waitingForNext)
                                {
                                    after = startTime.secsTo(QDateTime::fromString(mObj["TimePressed"].toString())) / binSize;
                                    waitingForNext = false;

                                    int fIndex = keyList.indexOf(mObj["KeyDescription"].toString());

                                    for (int k(prev); k <= after; k++)
                                    {
                                        scoringBins[fIndex][k] = 1;
                                    }
                                }
                                else
                                {
                                    prev = startTime.secsTo(QDateTime::fromString(mObj["TimePressed"].toString())) / binSize;
                                    waitingForNext = true;
                                }
                            }
                        }

                        if (waitingForNext)
                        {
                            int fIndex = keyList.indexOf(mScoreKey.at(i).first);

                            for (int k(prev); k < scoringBins[fIndex].count(); k++)
                            {
                                scoringBins[fIndex][k] = 1;
                            }
                        }
                    }
                }
            }

            for (int i(0); i<mScoreKey.count(); i++)
            {
                for (int j(0); j<mScoreKey.count(); j++)
                {
                    if (i != j)
                    {
                        QPair<LagCoding, LagCoding> lag;

                        ScoreLagCodings(scoringBins[i], scoringBins[j], &windowSize, &lag);

                        tableConstruction[i][j].first.InsideWindow += lag.first.InsideWindow;
                        tableConstruction[i][j].first.OutsideWindow += lag.first.OutsideWindow;
                        tableConstruction[i][j].second.InsideWindow += lag.second.InsideWindow;
                        tableConstruction[i][j].second.OutsideWindow += lag.second.OutsideWindow;
                    }
                }
            }
        }
    }

    for (int i(0); i<tableConstruction.count(); i++)
    {
        temp.clear();

        for (int j(0); j<tableConstruction[i].count(); j++)
        {
            if (i == j)
            {
                temp << QString("---");
            }
            else
            {
                if (analysisType == ProbabilityType::YulesQ)
                {
                    int A = tableConstruction[i][j].first.InsideWindow;
                    int B = tableConstruction[i][j].second.InsideWindow;
                    int C = tableConstruction[i][j].first.OutsideWindow;
                    int D = tableConstruction[i][j].second.OutsideWindow;

                    double Q = (((double) A * (double) D) - ((double) B * (double) C)) /
                               (((double) A * (double) D) + ((double) B * (double) C));

                    temp << QString::number(Q);
                }
                else if (analysisType == ProbabilityType::OperantContingencyValue)
                {
                    int A = tableConstruction[i][j].first.InsideWindow;
                    int B = tableConstruction[i][j].second.InsideWindow;
                    int C = tableConstruction[i][j].first.OutsideWindow;
                    int D = tableConstruction[i][j].second.OutsideWindow;

                    double OCV = (((double) A / ((double) A + (double) B)) -
                                  ((double) C / ((double) C + (double) D)));

                    temp << QString::number(OCV);
                }
                else if (analysisType == ProbabilityType::OddsRatio)
                {
                    int A = tableConstruction[i][j].first.InsideWindow;
                    int B = tableConstruction[i][j].second.InsideWindow;
                    int C = tableConstruction[i][j].first.OutsideWindow;
                    int D = tableConstruction[i][j].second.OutsideWindow;

                    double OR = (((double) A * (double) D) / ((double) B * (double) C));

                    temp << QString::number(OR);
                }
                else
                {
                    temp << QString("---");
                }
            }
        }

        mResults->append(temp);
    }
}

/**
 * @brief CreateContingencyTables
 * @param filePath
 * @param keyScoreList
 * @param mResults
 * @param code
 * @param binSize
 * @param windowSize
 */
static void CreateContingencyTables(QString filePath,
                                    QList<QPair<QString, int>> keyScoreList,
                                    QList<QStringList> * mResults,
                                    ProbabilityType analysisType, int binSize = 1, int windowSize = 4)
{
    QFile mSession(filePath);
    QJsonObject json;

    int sessionDuration;
    int secs;
    int binCount;
    int keyCount;

    QStringList temp;
    double holder;

    mResults->clear();

    QList<QList<int>> scoringBins;
    QDateTime startTime, endTime;
    QStringList tempKeyLookups;

    for (int i(0); i<keyScoreList.count(); i++)
    {
        tempKeyLookups << keyScoreList.at(i).first;
    }

    if (mSession.exists())
    {
        if (mSession.open(QIODevice::ReadOnly | QIODevice::Text))
        {
            QString sessionData = mSession.readAll();
            mSession.close();

            QJsonDocument loadSession = QJsonDocument::fromJson(sessionData.toUtf8());
            json = loadSession.object();

            sessionDuration = (json["SessionDuration"].toInt() / 1000);
            startTime = QDateTime(QDateTime::fromString(json["StartTime"].toString()));
            binCount = sessionDuration / binSize;
            keyCount = keyScoreList.count();

            for (int i(0); i< keyCount; i++)
            {
                scoringBins.append(QList<int>());

                for (int j(0); j < binCount; j++)
                {
                    scoringBins[i].append(0);
                }
            }

            // Frequencies here
            QJsonArray pressedKeysJson = json["PressedKeys"].toArray();
            foreach (const QJsonValue keyObj, pressedKeysJson) {
                QJsonObject mObj = keyObj.toObject();

                int fIndex = tempKeyLookups.indexOf(mObj["KeyDescription"].toString());

                if (fIndex != -1 && keyScoreList.at(fIndex).second == 1)
                {
                    secs = startTime.secsTo(QDateTime::fromString(mObj["TimePressed"].toString()));
                    int locationInArray = secs / binSize;

                    scoringBins[fIndex][locationInArray] = 1;
                }
            }

            // durations here
            bool waitingForNext = false;
            int prev, after;

            for (int i(0); i<keyScoreList.count(); i++)
            {
                // Is a duration key
                if (keyScoreList.at(i).second == 2)
                {
                    foreach (const QJsonValue keyObj, pressedKeysJson) {
                        QJsonObject mObj = keyObj.toObject();

                        if (mObj["KeyDescription"].toString() == keyScoreList.at(i).first)
                        {
                            if (waitingForNext)
                            {
                                after = startTime.secsTo(QDateTime::fromString(mObj["TimePressed"].toString())) / binSize;
                                waitingForNext = false;

                                int fIndex = tempKeyLookups.indexOf(mObj["KeyDescription"].toString());

                                for (int k(prev); k <= after; k++)
                                {
                                    scoringBins[fIndex][k] = 1;
                                }
                            }
                            else
                            {
                                prev = startTime.secsTo(QDateTime::fromString(mObj["TimePressed"].toString())) / binSize;
                                waitingForNext = true;
                            }
                        }
                    }

                    if (waitingForNext)
                    {
                        int fIndex = tempKeyLookups.indexOf(keyScoreList.at(i).first);

                        for (int k(prev); k < scoringBins[fIndex].count(); k++)
                        {
                            scoringBins[fIndex][k] = 1;
                        }
                    }
                }
            }
        }

        for (int i(0); i<keyScoreList.count(); i++)
        {
            temp.clear();

            for (int j(0); j<keyScoreList.count(); j++)
            {
                if (i == j)
                {
                    temp << QString("---");
                }
                else
                {
                    holder = ProbabilityTools::CountObjects(scoringBins[i], scoringBins[j], &windowSize, analysisType);
                    temp << QString::number(holder);
                }
            }

            mResults->append(temp);
        }
    }
}

/**
 * @brief ScoreLagCodings
 * @param preLagList
 * @param postLagList
 * @param windowSize
 * @param pair
 */
static void ScoreLagCodings(QList<int> preLagList, QList<int> postLagList, int * windowSize, QPair<LagCoding, LagCoding> * pair)
{
    int remainingLags = 0;
    bool inWindow = false;

    for (int i(0); i<preLagList.count() - (*windowSize - 1); i++)
    {
        inWindow = (remainingLags > 0);

        if (inWindow)
        {
            // A = A represents the number of seconds in which the ?target? occurred within the specified time window
            //int A = tableConstruction[i][j].first.InsideWindow;

            // B represents the number of seconds within the specified time window that did not also contain a ?target? code
            //int B = tableConstruction[i][j].second.InsideWindow;

            // C represents the number of seconds in which the ?target? code occurred outside the specified window,
            //int C = tableConstruction[i][j].first.OutsideWindow;

            // D represents the number of seconds in which neither the ?target? code nor the window code occurred
            //int D = tableConstruction[i][j].second.OutsideWindow;

            if (postLagList.at(i) == 1)
            {
                pair->first.InsideWindow++;
            }
            else
            {
                pair->second.InsideWindow++;
            }
        }
        else
        {
            if (postLagList.at(i) == 1)
            {
                pair->first.OutsideWindow++;
            }
            else
            {
                pair->second.OutsideWindow++;
            }
        }

        remainingLags--;

        if (remainingLags < 0)
        {
            remainingLags = 0;
        }

        if (preLagList.at(i) == 1)
        {
            remainingLags = *windowSize;
        }
    }
}

/**
 * @brief CountObjects
 * @param preLagList
 * @param postLagList
 * @param windowSize
 * @param code
 * @return
 */
static double CountObjects(QList<int> preLagList, QList<int> postLagList, int * windowSize, ProbabilityType analysisType)
{
    LagCoding hasBxLag;
    LagCoding noBxLag;

    int remainingLags = 0;
    bool inWindow = false;

    for (int i(0); i<preLagList.count() - (*windowSize - 1); i++)
    {
        inWindow = (remainingLags > 0);

        if (inWindow)
        {
            if (postLagList.at(i) == 1)
            {
                hasBxLag.InsideWindow++;
            }
            else
            {
                noBxLag.InsideWindow++;
            }
        }
        else
        {
            if (postLagList.at(i) == 1)
            {
                hasBxLag.OutsideWindow++;
            }
            else
            {
                noBxLag.OutsideWindow++;
            }
        }

        remainingLags--;

        if (remainingLags < 0)
        {
            remainingLags = 0;
        }

        if (preLagList.at(i) == 1)
        {
            remainingLags = *windowSize;
        }
    }

    int A = hasBxLag.InsideWindow;
    int B = noBxLag.InsideWindow;
    int C = hasBxLag.OutsideWindow;
    int D = noBxLag.OutsideWindow;

    if (analysisType == ProbabilityType::OddsRatio)
    {
        double OR = (((double) A * (double) D) / ((double) B * (double) C));

        return OR;
    }
    else if (analysisType == ProbabilityType::YulesQ)
    {
        double Q = (((double) A * (double) D) - ((double) B * (double) C)) /
                   (((double) A * (double) D) + ((double) B * (double) C));

        return Q;
    }
    else if (analysisType == ProbabilityType::OperantContingencyValue)
    {
        double OCV = (((double) A / ((double) A + (double) B)) -
                      ((double) C / ((double) C + (double) D)));

        return OCV;
    }
    else
    {
        return -1;
    }
}

};

#endif // PROBABILITYTOOLS_H
