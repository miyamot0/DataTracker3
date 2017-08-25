#ifndef DIRECTORYSEARCHER_H
#define DIRECTORYSEARCHER_H

#include <QObject>

#include "directoryparse.h"
#include "parsetypes.h"

class DirectorySearcher : public QObject
{
    Q_OBJECT

public:
    DirectorySearcher(DirectoryParse mPresent);

signals:
    void workStarted();
    void workingResult(const QString &value);
    void workFinished(const DirectoryParse mParsedDirectory, ParseTypes::ParseAction mAction);

public slots:
    void startWork();
    void working();

private:
    DirectoryParse currentDirectory;
};

#endif // DIRECTORYSEARCHER_H
