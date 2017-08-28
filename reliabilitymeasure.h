#ifndef RELIABILITYMEASURE_H
#define RELIABILITYMEASURE_H

#include <QObject>
#include "keysetentry.h"

class ReliabilityMeasure
{
public:
    int Session;
    QString Group;
    QString Individual;
    QString Evaluation;
    QString Condition;

    QString Primary;
    QString Reliability;

    QList<KeySetEntry> FrequencyKeys;

    QList<QPair<QString, QString>> fEIA;
    QList<QPair<QString, QString>> fPIA;
    QList<QPair<QString, QString>> fTIA;
    QList<QPair<QString, QString>> fOIA;
    QList<QPair<QString, QString>> fNIA;
    QList<QPair<QString, QString>> fPMA;

    QList<KeySetEntry> DurationKeys;

    QList<QPair<QString, QString>> dEIA;
    QList<QPair<QString, QString>> dPIA;
    QList<QPair<QString, QString>> dTIA;
    QList<QPair<QString, QString>> dOIA;
    QList<QPair<QString, QString>> dNIA;
    QList<QPair<QString, QString>> dPMA;

};

#endif // RELIABILITYMEASURE_H
