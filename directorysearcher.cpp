#include "directoryparse.h"
#include "directorysearcher.h"

#include "filetools.h"

#include <QDir>
#include <QJsonDocument>

#include <QDebug>

DirectorySearcher::DirectorySearcher(DirectoryParse mPresent)
{
    currentDirectory = mPresent;
}

void DirectorySearcher::startWork()
{
    emit workStarted();
}

void DirectorySearcher::working()
{
    emit workingResult("Working...");

    DirectoryParse mReturn;

    if (currentDirectory.CurrentGroup.length() == 0)
    {
        QDir workingDir(currentDirectory.WorkingDirectory);

        QFileInfoList mEntries = workingDir.entryInfoList(QDir::NoDotAndDotDot |
                                                          QDir::System |
                                                          QDir::Hidden  |
                                                          QDir::AllDirs |
                                                          QDir::Files,
                                                          QDir::DirsFirst);

        for (QFileInfo mInf : mEntries)
        {
            if (mInf.isDir())
            {
                mReturn.Groups << mInf.fileName();
            }
        }

        emit workFinished(mReturn, ParseTypes::ParseAction::Group);

        return;
    }

    if (currentDirectory.CurrentIndividual.length() == 0)
    {
        QDir workingDir(FileTools::pathAppend(currentDirectory.WorkingDirectory, currentDirectory.CurrentGroup));

        QFileInfoList mEntries = workingDir.entryInfoList(QDir::NoDotAndDotDot |
                                                          QDir::System |
                                                          QDir::Hidden  |
                                                          QDir::AllDirs |
                                                          QDir::Files,
                                                          QDir::DirsFirst);

        for (QFileInfo mInf : mEntries)
        {
            if (mInf.isDir())
            {
                mReturn.Individuals << mInf.fileName();
            }
        }

        emit workFinished(mReturn, ParseTypes::ParseAction::Individual);

        return;
    }

    if (currentDirectory.CurrentEvaluation.length() == 0)
    {
        // Load evaluations

        QString mFile = FileTools::pathAppend(currentDirectory.WorkingDirectory, currentDirectory.CurrentGroup);
        QDir workingDir(FileTools::pathAppend(mFile, currentDirectory.CurrentIndividual));

        QFileInfoList mEntries = workingDir.entryInfoList(QDir::NoDotAndDotDot |
                                                          QDir::System |
                                                          QDir::Hidden  |
                                                          QDir::AllDirs |
                                                          QDir::Files,
                                                          QDir::DirsFirst);

        for (QFileInfo mInf : mEntries)
        {
            if (mInf.isDir())
            {
                mReturn.Evaluations << mInf.fileName();
            }
        }

        // Load key sets

        QStringList filters;
        filters << "*.json";

        QDir jsonLocation = QDir(workingDir);
        jsonLocation.setNameFilters(filters);
        jsonLocation.setFilter(QDir::Files);

        mEntries = jsonLocation.entryInfoList();

        QStringList mSplit;

        for (QFileInfo mInf : mEntries)
        {
            if (mInf.fileName() != "Therapists.json" &&
                mInf.fileName() != "Collectors.json")
            {
                mSplit = mInf.fileName().split(".");
                mReturn.KeySets << mSplit[0];
            }
        }

        // Therapist Parsing
        QString mTherapistJson = FileTools::pathAppend(FileTools::pathAppend(mFile, currentDirectory.CurrentIndividual), "Therapists.json");
        FileTools::ReadTherapists(mTherapistJson, &mReturn.Therapists);

        // Collector Parsing
        QString mCollectorJson = FileTools::pathAppend(FileTools::pathAppend(mFile, currentDirectory.CurrentIndividual), "Collectors.json");
        FileTools::ReadCollectors(mCollectorJson, &mReturn.Collectors);

        emit workFinished(mReturn, ParseTypes::ParseAction::Evaluation);

        return;
    }

    if (currentDirectory.CurrentCondition.length() == 0)
    {
        QString mFile = FileTools::pathAppend(currentDirectory.WorkingDirectory, currentDirectory.CurrentGroup);
        mFile = FileTools::pathAppend(mFile, currentDirectory.CurrentIndividual);
        QDir workingDir(FileTools::pathAppend(mFile, currentDirectory.CurrentEvaluation));

        QFileInfoList mEntries = workingDir.entryInfoList(QDir::NoDotAndDotDot |
                                                          QDir::System |
                                                          QDir::Hidden  |
                                                          QDir::AllDirs |
                                                          QDir::Files,
                                                          QDir::DirsFirst);

        for (QFileInfo mInf : mEntries)
        {
            if (mInf.isDir())
            {
                mReturn.Conditions << mInf.fileName();
            }
        }

        emit workFinished(mReturn, ParseTypes::ParseAction::Condition);

        return;
    }

    emit workFinished(currentDirectory, ParseTypes::ParseAction::None);
}
