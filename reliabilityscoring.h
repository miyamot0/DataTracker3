#ifndef RELIABILITYSCORING_H
#define RELIABILITYSCORING_H

#include <QObject>

class ReliabilityScoring
{
public:

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
    if (mPrimary->count() == 0 || mSecondary.count() == 0)
        return -1;

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
    if (mPrimary->count() == 0 || mSecondary.count() == 0)
        return -1;

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
    if (mPrimary->count() == 0 || mSecondary.count() == 0)
        return -1;

    int runLength = GetLengthCompare(&mPrimary, &mSecondary);

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
            higher = (mPrimary[i] > mSecondary[i]) ? mPrimary[i] : mSecondary[i];
            lower = (mPrimary[i] > mSecondary[i]) ? mSecondary[i] : mPrimary[i];

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
    if (mPrimary->count() == 0 || mSecondary.count() == 0)
        return -1;

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
    if (mPrimary->count() == 0 || mSecondary.count() == 0)
        return -1;

    int runLength = GetLengthCompare(&mPrimary, &mSecondary);

    int count = 0;
    double sum = 0.0;

    for (int i(0); i < runLength; i++)
    {
        if (mPrimary[i] == 0 && mSecondary[i] == 0)
        {
            sum += 1.0;
        }
        else if (mPrimary[i] > 0 && mSecondary[i] > 0)
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
    if (mPrimary->count() == 0 || mSecondary.count() == 0)
        return -1;

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
    if (mPrimary->count() == 0 || mSecondary.count() == 0)
        return -1;

    int runLength = GetLengthCompare(&mPrimary, &mSecondary);

    int count = 0;
    double sum = 0.0;

    for (int i(0); i < runLength; i++)
    {
        if (mPrimary[i] > 0 || mSecondary[i] > 0)
        {
            if (mPrimary[i] == 0 && mSecondary[i] == 0)
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
    if (mPrimary->count() == 0 || mSecondary.count() == 0)
        return -1;

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
    if (mPrimary->count() == 0 || mSecondary.count() == 0)
        return -1;

    int runLength = GetLengthCompare(&mPrimary, &mSecondary);

    int count = 0;
    double sum = 0.0;

    for (int i(0); i < runLength; i++)
    {
        if (mPrimary[i] < 1 || mSecondary[i] < 1)
        {
            if (mPrimary[i] == 0 && mSecondary[i] == 0)
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
    if (mPrimary->count() == 0 || mSecondary.count() == 0)
        return -1;

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
    if (mPrimary->count() == 0 || mSecondary.count() == 0)
        return -1;

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
        innerPrim += mPrimary[i];
        innerReli += mSecondary[i];

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
    if (mPrimary->count() == 0 || mSecondary.count() == 0)
        return -1;

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


    return (sum / (((double)count))) * 100;
}

}

#endif // RELIABILITYSCORING_H
