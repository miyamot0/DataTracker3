#ifndef RELIABILITYPARSE_H
#define RELIABILITYPARSE_H

#include <QObject>

class ReliabilityParse
{

public:
    int SessionNumber;

    QString Condition;
    QString Collector;
    QString SecondaryObserver;

    QString PrimaryFilePath;
    QString ReliFilePath;

    bool Reli;

    int Row;

};

#endif // RELIABILITYPARSE_H
