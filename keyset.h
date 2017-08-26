#ifndef KEYSET_H
#define KEYSET_H

#include <QObject>

#include "keysetentry.h"

class KeySet
{
public:
    QString KeySetName;

    QList<KeySetEntry> FrequencyKeys;
    QList<KeySetEntry> DurationKeys;

    int TotalSeconds;
    int Session;
};

#endif // KEYSET_H
