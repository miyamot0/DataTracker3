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

class FileTools
{
public:

static void ReadKeySet(QString path, KeySet * keySet)
{
    QFile mKeySet(path);

    if (mKeySet.exists())
    {
        if (mKeySet.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString keySetData = mKeySet.readAll();
            mKeySet.close();

            QJsonDocument loadKeySets = QJsonDocument::fromJson(keySetData.toUtf8());
            QJsonObject  keySetObject = loadKeySets.object();

            keySet->KeySetName = keySetObject["Title"].toString();

            QJsonArray frequencyArray = keySetObject["FrequencyKeys"].toArray();

            for (int i=0; i<frequencyArray.size(); i++)
            {
                QJsonObject keyObject = frequencyArray[i].toObject();

                KeySetEntry newKeys;
                newKeys.KeyCode = keyObject["Code"].toInt();
                newKeys.KeyName = keyObject["Name"].toString();
                newKeys.KeyDescription = keyObject["Description"].toString();

                keySet->FrequencyKeys.append(newKeys);

            }

            QJsonArray durationArray = keySetObject["DurationKeys"].toArray();

            for (int i=0; i<durationArray.size(); i++)
            {
                QJsonObject keyObject = durationArray[i].toObject();

                KeySetEntry newKeys;
                newKeys.KeyCode = keyObject["Code"].toInt();
                newKeys.KeyName = keyObject["Name"].toString();
                newKeys.KeyDescription = keyObject["Description"].toString();

                keySet->DurationKeys.append(newKeys);

            }
        }
    }
}

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

static void ReadTherapists(QString path, QStringList * therapists)
{
    QFile mTherapists(path);

    if (mTherapists.exists())
    {
        if (mTherapists.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString therapistData = mTherapists.readAll();
            mTherapists.close();

            QJsonDocument loadTherapists = QJsonDocument::fromJson(therapistData.toUtf8());
            QJsonObject  therapistObject = loadTherapists.object();
            QJsonArray therapistArray = therapistObject["Therapists"].toArray();

            foreach (const QJsonValue therapist, therapistArray) {
                *therapists << therapist.toString();
            }
        }
    }
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

static void ReadCollectors(QString path, QStringList * collectors)
{
    QFile mCollectors(path);

    if (mCollectors.exists())
    {
        if (mCollectors.open(QIODevice::ReadOnly | QIODevice::Text)) {
            QString collectorData = mCollectors.readAll();
            mCollectors.close();

            QJsonDocument loadCollectors = QJsonDocument::fromJson(collectorData.toUtf8());
            QJsonObject  collectorObject = loadCollectors.object();
            QJsonArray collectorArray = collectorObject["Collectors"].toArray();

            foreach (const QJsonValue collector, collectorArray) {
                *collectors << collector.toString();
            }
        }
    }
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
