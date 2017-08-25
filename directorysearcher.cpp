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

    emit workingResult(currentDirectory.WorkingDirectory);

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


    emit workFinished(currentDirectory, ParseTypes::ParseAction::None);
}
