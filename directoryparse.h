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

    QStringList Groups;
    QStringList Individuals;
    QStringList Evaluations;
    QStringList Conditions;

    //QString Individual;
    //QString Evaluation;
    //QString Condition;

    //KeySet

    QString Therapist;
    bool Primary;
    int Duration;
    int Session;
};

#endif // DIRECTORYPARSE_H
