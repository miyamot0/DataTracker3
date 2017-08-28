#ifndef RELIABILITYSCORING_H
#define RELIABILITYSCORING_H

#include <QObject>

#include "keysetentry.h"
#include "sessionevent.h"

class ReliabilityScoring
{
public:

static QList<QList<int>> GetFrequencyBins(int bins, QList<KeySetEntry> FrequencyKeys, QDateTime startTime, QList<SessionEvent> PressedKeys)
{
    QList<QList<int>> mFrequencyBins;
    int fKeys = FrequencyKeys.count();

    for (int i(0); i<bins; i++)
    {
        mFrequencyBins.append(QList<int>());

        for (int j(0); j<fKeys; j++)
        {
            mFrequencyBins[i].append(0);
        }
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
                //mFrequencyBins[i][timeHolder] = mFrequencyBins[i][timeHolder] + 1;
                mFrequencyBins[timeHolder][i] = mFrequencyBins[timeHolder][i] + 1;
            }
        }
    }


    return mFrequencyBins;
}

static QList<QList<double>> GetDurationBins(int bins, QList<KeySetEntry> DurationKeys, QDateTime startTime, QDateTime endTime, QList<SessionEvent> PressedKeys)
{
    QList<QList<double>> mDurationBins;
    int dKeys = DurationKeys.count();

    for (int i(0); i<bins; i++)
    {
        mDurationBins.append(QList<double>());

        for (int j(0); j<dKeys; j++)
        {
            mDurationBins[i].append(0.0);
        }
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
                //mDurationBins[i][index1] = mDurationBins[i][index1] + ((double)(stop - start))/1000;
                mDurationBins[index1][i] = mDurationBins[index1][i] + ((double)(stop - start))/1000;
            }
            else if ((index2 - index1) == 1)
            {
                //mDurationBins[i][index1] = mDurationBins[i][index1] + ((double)(10000 - (start % 10000)))/1000;
                //mDurationBins[i][index2] = mDurationBins[i][index2] + ((double)(stop % 10000))/1000;
                mDurationBins[index1][i] = mDurationBins[index1][i] + ((double)(10000 - (start % 10000)))/1000;
                mDurationBins[index2][i] = mDurationBins[index2][i] + ((double)(stop % 10000))/1000;
            }
            else
            {
                //mDurationBins[i][index1] = mDurationBins[i][index1] + ((double)(10000 - (start % 10000)))/1000;
                mDurationBins[index1][i] = mDurationBins[index1][i] + ((double)(10000 - (start % 10000)))/1000;

                for (int k(index1 + 1); k < index2; k++)
                {
                    //mDurationBins[i][k] = mDurationBins[i][k] + 10;
                    mDurationBins[k][i] = mDurationBins[k][i] + 10;
                }

                //mDurationBins[i][index2] = mDurationBins[i][index2] + ((double)(stop % 10000))/1000;
                mDurationBins[index2][i] = mDurationBins[index2][i] + ((double)(stop % 10000))/1000;
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
 * @brief getFrequenyEIA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static double getFrequenyEIA(QList<double> * mPrimary, QList<double> * mSecondary)
{
    if ((mPrimary->count() == 0))
    {
        return -1;
    }

    if ((mSecondary->count() == 0))
    {
        return -1;
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

    return ((double) sum / (double) count) * 100;
}

/**
 * @brief getDurationEIA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static double getDurationEIA(QList<double> * mPrimary, QList<double> * mSecondary)
{
    if ((mPrimary->count() == 0))
    {
        return -1;
    }

    if ((mSecondary->count() == 0))
    {
        return -1;
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

    return ((double) sum / (double) count) * 100;
}

/**
 * @brief getFreqPIA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static double getFreqPIA(QList<double> * mPrimary, QList<double> * mSecondary)
{
    if ((mPrimary->count() == 0))
    {
        return -1;
    }

    if ((mSecondary->count() == 0))
    {
        return -1;
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

    return ( sum / (double) count) * 100;
}


/**
 * @brief getDurationPIA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static double getDurationPIA(QList<double> * mPrimary, QList<double> * mSecondary)
{
    if ((mPrimary->count() == 0))
    {
        return -1;
    }

    if ((mSecondary->count() == 0))
    {
        return -1;
    }

    int runLength = GetLengthCompare(mPrimary, mSecondary);

    double higher, lower;

    int count = 0;
    double sum = 0.0;

    for (int i(0); i < runLength; i++)
    {
        if (qRound(mPrimary->at(i)) == qRound(mSecondary->at(i)))
        {
            sum += 1.0;
        }
        else
        {
            higher = (mPrimary->at(i) > mSecondary->at(i)) ? mPrimary->at(i) : mSecondary->at(i);
            lower = (mPrimary->at(i) > mSecondary->at(i)) ? mSecondary->at(i) : mPrimary->at(i);

            sum += (lower / higher);
        }

        count++;
    }

    return ( sum / (double) count) * 100;
}

/**
 * @brief getFreqTIA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static double getFreqTIA(QList<double> * mPrimary, QList<double> * mSecondary)
{
    if ((mPrimary->count() == 0))
    {
        return -1;
    }

    if ((mSecondary->count() == 0))
    {
        return -1;
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

    return ( sum / (double) count) * 100;
}

/**
 * @brief getDurationTIA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static double getDurationTIA(QList<double> * mPrimary, QList<double> * mSecondary)
{
    if ((mPrimary->count() == 0))
    {
        return -1;
    }

    if ((mSecondary->count() == 0))
    {
        return -1;
    }

    int runLength = GetLengthCompare(mPrimary, mSecondary);

    int count = 0;
    double sum = 0.0;

    for (int i(0); i < runLength; i++)
    {
        if (qRound(mPrimary->at(i)) == 0 && qRound(mSecondary->at(i)) == 0)
        {
            sum += 1.0;
        }
        else if (mPrimary->at(i) > 0 && mSecondary->at(i) > 0)
        {
            sum += 1.0;
        }

        count++;
    }

    return ( sum / (double) count) * 100;
}

/**
 * @brief getFreqOIA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static double getFreqOIA(QList<double> * mPrimary, QList<double> * mSecondary)
{
    if ((mPrimary->count() == 0))
    {
        return -1;
    }

    if ((mSecondary->count() == 0))
    {
        return -1;
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

    return ( sum / (double) count) * 100;
}

/**
 * @brief getDurationOIA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static double getDurationOIA(QList<double> * mPrimary, QList<double> * mSecondary)
{
    if ((mPrimary->count() == 0))
    {
        return -1;
    }

    if ((mSecondary->count() == 0))
    {
        return -1;
    }

    int runLength = GetLengthCompare(mPrimary, mSecondary);

    int count = 0;
    double sum = 0.0;

    for (int i(0); i < runLength; i++)
    {
        if (mPrimary->at(i) > 0 || mSecondary->at(i) > 0)
        {
            if (qRound(mPrimary->at(i)) == 0 && qRound(mSecondary->at(i)) == 0)
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

    return ( sum / (double) count) * 100;
}

/**
 * @brief getFreqNIA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static double getFreqNIA(QList<double> * mPrimary, QList<double> * mSecondary)
{
    if ((mPrimary->count() == 0))
    {
        return -1;
    }

    if ((mSecondary->count() == 0))
    {
        return -1;
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

    return ( sum / (double) count) * 100;
}

/**
 * @brief getDurationNIA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static double getDurationNIA(QList<double> * mPrimary, QList<double> * mSecondary)
{
    if ((mPrimary->count() == 0))
    {
        return -1;
    }

    if ((mSecondary->count() == 0))
    {
        return -1;
    }

    int runLength = GetLengthCompare(mPrimary, mSecondary);

    int count = 0;
    double sum = 0.0;

    for (int i(0); i < runLength; i++)
    {
        if (mPrimary->at(i) < 1 || mSecondary->at(i) < 1)
        {
            if (qRound(mPrimary->at(i)) == 0 && qRound(mSecondary->at(i)) == 0)
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

    return ( sum / (double) count) * 100;
}

/**
 * @brief getFreqPMA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static double getFreqPMA(QList<double> * mPrimary, QList<double> * mSecondary)
{
    if ((mPrimary->count() == 0))
    {
        return -1;
    }

    if ((mSecondary->count() == 0))
    {
        return -1;
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

    return ( sum / (double) count) * 100;
}

/**
 * @brief getDurationPMA
 * @param mPrimary
 * @param mSecondary
 * @return
 */
static double getDurationPMA(QList<double> * mPrimary, QList<double> * mSecondary)
{
    if ((mPrimary->count() == 0))
    {
        return -1;
    }

    if ((mSecondary->count() == 0))
    {
        return -1;
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
        innerPrim += qRound(mPrimary->at(i));
        innerReli += qRound(mSecondary->at(i));

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


    return (sum / (((double)count))) * 100;
}


};

#endif // RELIABILITYSCORING_H
