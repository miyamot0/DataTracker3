#ifndef DIRECTORYPARSE_H
#define DIRECTORYPARSE_H

#include <QWidget>

class DirectoryParse
{
public:
    QString WorkingDirectory;

    QString CurrentGroup;
    QString CurrentIndividual;

    QStringList Groups;
    QStringList Individuals;

    QString Individual;
    QString Evaluation;
    QString Condition;

    //KeySet

    QString Therapist;
    bool Primary;
    int Duration;
    int Session;
};

#endif // DIRECTORYPARSE_H
