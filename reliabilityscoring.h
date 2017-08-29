#ifndef RELIABILITYSCORING_H
#define RELIABILITYSCORING_H

#include <QObject>

#include "keysetentry.h"
#include "sessionevent.h"
#include "reliabilitymeasure.h"

class ReliabilityScoring
{
public:

static void CompareObservers(QJsonObject mPrimary, QJsonObject mSecondary, ReliabilityMeasure * mMeasure)
{
    QDateTime startTime = QDateTime(QDateTime::fromString(mPrimary["StartTime"].toString()));
    QDateTime endTime = QDateTime(QDateTime::fromString(mPrimary["EndTime"].toString()));

    int totalSecs = (int)((double) mPrimary["SessionDuration"].toInt() / 1000);

    int bins = totalSecs / 10;
    int overflow = ((totalSecs % 10) > 0) ? 1 : 0;
        bins = bins + overflow;

    QList<KeySetEntry> FrequencyKeys;

    QJsonArray frequencyArray = mPrimary["FrequencyKeys"].toArray();
    foreach (const QJsonValue collector, frequencyArray) {
        QJsonObject mObj = collector.toObject();

        KeySetEntry mEntry;
        mEntry.KeyCode = mObj["Code"].toInt();
        mEntry.KeyDescription = mObj["Description"].toString();
        mEntry.KeyName = mObj["Name"].toString();

        FrequencyKeys.append(mEntry);
    }

    QList<KeySetEntry> DurationKeys;

    QJsonArray durationArray = mPrimary["DurationKeys"].toArray();
    foreach (const QJsonValue collector, durationArray) {
        QJsonObject mObj = collector.toObject();

        KeySetEntry mEntry;
        mEntry.KeyCode = mObj["Code"].toInt();
        mEntry.KeyDescription = mObj["Description"].toString();
        mEntry.KeyName = mObj["Name"].toString();

        DurationKeys.append(mEntry);
    }

    QList<SessionEvent> PressedKeys;

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

        PressedKeys.append(mEntry);
    }

    QList<QList<int>> mPrimaryFrequencyBins = ReliabilityScoring::GetFrequencyBins(bins, FrequencyKeys, startTime, PressedKeys);
    QList<QList<double>> mPrimaryDurationBins = ReliabilityScoring::GetDurationBins(bins, DurationKeys, startTime, endTime, PressedKeys);

    FrequencyKeys.clear();

    frequencyArray = mSecondary["FrequencyKeys"].toArray();
    foreach (const QJsonValue collector, frequencyArray) {
        QJsonObject mObj = collector.toObject();

        KeySetEntry mEntry;
        mEntry.KeyCode = mObj["Code"].toInt();
        mEntry.KeyDescription = mObj["Description"].toString();
        mEntry.KeyName = mObj["Name"].toString();

        FrequencyKeys.append(mEntry);
    }

    DurationKeys.clear();

    durationArray = mSecondary["DurationKeys"].toArray();
    foreach (const QJsonValue collector, durationArray) {
        QJsonObject mObj = collector.toObject();

        KeySetEntry mEntry;
        mEntry.KeyCode = mObj["Code"].toInt();
        mEntry.KeyDescription = mObj["Description"].toString();
        mEntry.KeyName = mObj["Name"].toString();

        DurationKeys.append(mEntry);
    }

    PressedKeys.clear();

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

        PressedKeys.append(mEntry);
    }

    QList<QList<int>> mSecondaryFrequencyBins   = ReliabilityScoring::GetFrequencyBins(bins, FrequencyKeys, startTime, PressedKeys);
    QList<QList<double>> mSecondaryDurationBins = ReliabilityScoring::GetDurationBins(bins, DurationKeys, startTime, endTime, PressedKeys);

    for (int i(0); i<FrequencyKeys.count(); i++)
    {
        mMeasure->fEIA.append(QPair<QString,QString>(FrequencyKeys[i].KeyName, getFrequencyEIA(&mPrimaryFrequencyBins[i], &mSecondaryFrequencyBins[i])));
        mMeasure->fPIA.append(QPair<QString,QString>(FrequencyKeys[i].KeyName, getFrequencyPIA(&mPrimaryFrequencyBins[i], &mSecondaryFrequencyBins[i])));
        mMeasure->fTIA.append(QPair<QString,QString>(FrequencyKeys[i].KeyName, getFrequencyTIA(&mPrimaryFrequencyBins[i], &mSecondaryFrequencyBins[i])));
        mMeasure->fOIA.append(QPair<QString,QString>(FrequencyKeys[i].KeyName, getFrequencyOIA(&mPrimaryFrequencyBins[i], &mSecondaryFrequencyBins[i])));
        mMeasure->fNIA.append(QPair<QString,QString>(FrequencyKeys[i].KeyName, getFrequencyNIA(&mPrimaryFrequencyBins[i], &mSecondaryFrequencyBins[i])));
        mMeasure->fPMA.append(QPair<QString,QString>(FrequencyKeys[i].KeyName, getFrequencyPMA(&mPrimaryFrequencyBins[i], &mSecondaryFrequencyBins[i])));
    }

    for (int i(0); i<DurationKeys.count(); i++)
    {
        mMeasure->dEIA.append(QPair<QString,QString>(DurationKeys[i].KeyName, getDurationEIA(mPrimaryDurationBins[i], mSecondaryDurationBins[i])));
        mMeasure->dPIA.append(QPair<QString,QString>(DurationKeys[i].KeyName, getDurationPIA(mPrimaryDurationBins[i], mSecondaryDurationBins[i])));
        mMeasure->dTIA.append(QPair<QString,QString>(DurationKeys[i].KeyName, getDurationTIA(mPrimaryDurationBins[i], mSecondaryDurationBins[i])));
        mMeasure->dOIA.append(QPair<QString,QString>(DurationKeys[i].KeyName, getDurationOIA(mPrimaryDurationBins[i], mSecondaryDurationBins[i])));
        mMeasure->dNIA.append(QPair<QString,QString>(DurationKeys[i].KeyName, getDurationNIA(mPrimaryDurationBins[i], mSecondaryDurationBins[i])));
        mMeasure->dPMA.append(QPair<QString,QString>(DurationKeys[i].KeyName, getDurationPMA(mPrimaryDurationBins[i], mSecondaryDurationBins[i])));
    }
}

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