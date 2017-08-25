#ifndef FILETOOLS_H
#define FILETOOLS_H

#include <QStandardPaths>
#include <QObject>
#include <QDir>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include "directoryparse.h"
#include "parsetypes.h"
#include "keyset.h"
#include "keysetentry.h"


#include <QDebug>


class FileTools
{
public:

static void WriteKeySet(QString path, KeySet keySet)
{
    QJsonObject json;
    json["Title"] = keySet.KeySetName;

    QJsonArray frequencyKeys;
    foreach(KeySetEntry entry, keySet.FrequencyKeys)
    {
        QJsonObject mEntry;
        mEntry["Code"] = entry.KeyCode;
        mEntry["Name"] = entry.KeyName;
        mEntry["Description"] = entry.KeyDescription;

        frequencyKeys.append(mEntry);
    }
    json["FrequencyKeys"] = frequencyKeys;

    QJsonArray durationKeys;
    foreach(KeySetEntry entry, keySet.DurationKeys)
    {
        QJsonObject mEntry;
        mEntry["Code"] = entry.KeyCode;
        mEntry["Name"] = entry.KeyName;
        mEntry["Description"] = entry.KeyDescription;

        durationKeys.append(mEntry);
    }
    json["DurationKeys"] = durationKeys;

    QJsonDocument jsonDoc(json);

    QFile saveFile(path);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        // TODO error handling
        return;
    }

    saveFile.write(jsonDoc.toJson());
}

static void WriteTherapists(QString path, QStringList therapists)
{
    QJsonObject json;

    QJsonArray therapistObj;

    foreach(QString entry, therapists)
    {
        therapistObj.append(entry);
    }
    json["Therapists"] = therapistObj;

    QJsonDocument jsonDoc(json);

    QFile saveFile(path);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        // TODO error handling

        return;
    }

    saveFile.write(jsonDoc.toJson());
}

static void WriteCollectors(QString path, QStringList collectors)
{
    QJsonObject json;

    QJsonArray collectorObj;

    foreach(QString entry, collectors)
    {
        collectorObj.append(entry);
    }
    json["Collectors"] = collectorObj;

    QJsonDocument jsonDoc(json);

    QFile saveFile(path);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        // TODO error handling

        return;
    }

    saveFile.write(jsonDoc.toJson());
}

static QString pathAppend(const QString& path, const QString& file)
{
    return QDir::cleanPath(path + QDir::separator() + file);
}

static QStringList ParseDirectory(DirectoryParse mDirectory, ParseTypes)
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
