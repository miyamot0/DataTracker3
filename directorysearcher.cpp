/**
   Copyright 2017 Shawn Gilroy

   This file is part of Data Tracker, Qt port.

   Discounting Model Selector is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 3.

   Discounting Model Selector is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Discounting Model Selector.  If not, see http://www.gnu.org/licenses/.

   The Discounting Model Selector is a tool to assist researchers in behavior economics.

   Email: shawn(dot)gilroy(at)temple.edu

  */

#include "directoryparse.h"
#include "directorysearcher.h"

#include "filetools.h"

#include <QDir>
#include <QJsonDocument>

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
