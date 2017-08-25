#ifndef DIRECTORYPARSE_H
#define DIRECTORYPARSE_H

#include <QWidget>

class DirectoryParse
{
public:
    QString WorkingDirectory;

    QString CurrentGroup;
    QString CurrentIndividual;
    QString CurrentEvaluation;
    QString CurrentCondition;
    QString CurrentKeySet;
    QString CurrentTherapist;
    QString CurrentCollector;

    QStringList Groups;
    QStringList Individuals;
    QStringList Evaluations;
    QStringList Conditions;
    QStringList KeySets;
    QStringList Therapists;
    QStringList Collectors;

    bool Primary;
    int Duration;
    int Session;
};

#endif // DIRECTORYPARSE_H
