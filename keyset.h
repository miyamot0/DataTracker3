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

};

#endif // KEYSET_H
