#ifndef FILETOOLS_H
#define FILETOOLS_H

#include <QStandardPaths>
#include <QObject>
#include <QDir>

#include "directoryparse.h"
#include "parsetypes.h"

class FileTools
{
public:

static QString pathAppend(const QString& path, const QString& file)
{
    return QDir::cleanPath(path + QDir::separator() + file);
}

static QStringList ParseDirectory(DirectoryParse mDirectory, ParseTypes mAction)
{
    QStringList mParsedDirectories;

    return mParsedDirectories;
}

static bool CheckAndPrepDirectory(QString folderTitle)
{
    QStringList mDocFolder = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation);

    if (mDocFolder.count() > 0)
    {
        QDir recoredDir(mDocFolder[0]);
        QFileInfoList mEntries = recoredDir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);

        if (!FileTools::CheckDirectory(mEntries, folderTitle))
        {
            recoredDir.mkdir(folderTitle);
        }

        if (FileTools::CheckDirectory(mEntries, folderTitle))
        {
            return true;
        }
    }

    return false;
}

static bool CheckDirectory(QFileInfoList mEntries, QString folderTitle)
{
    for (QFileInfo mInf : mEntries)
    {
        if (mInf.isDir())
        {
            if (mInf.fileName() == folderTitle)
            {
                return true;
            }
        }
    }

    return false;
}

};

#endif // FILETOOLS_H
