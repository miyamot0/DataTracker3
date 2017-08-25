#include "directoryparse.h"
#include "directorysearcher.h"

#include "filetools.h"

#include <QDir>

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
