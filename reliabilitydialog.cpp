/**
   Copyright 2017 Shawn Gilroy

   This file is part of Data Tracker, Qt port.

   Data Tracker is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 3.

   Data Tracker is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Data Tracker.  If not, see http://www.gnu.org/licenses/.

   The Data Tracker is a tool to assist researchers in behavior economics.

   Email: shawn(dot)gilroy(at)temple.edu

  */

#include "reliabilitydialog.h"
#include "ui_reliabilitydialog.h"

#include "directorysearcher.h"
#include "filetools.h"
#include "reliabilityparse.h"

#include <QObject>
#include <QDebug>
#include <QThread>

ReliabilityDialog::ReliabilityDialog(QString mCurrentWorkingDirectory, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ReliabilityDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("Reliability Calculator"));

    workerThread = new QThread();

    mWorkingDirectory = mCurrentWorkingDirectory;

    mCurrentDirectory.WorkingDirectory = mWorkingDirectory;

    worker = new DirectorySearcher(mCurrentDirectory);

    worker->moveToThread(workerThread);

    connect(worker, SIGNAL(workStarted()), workerThread, SLOT(start()));
    connect(workerThread, SIGNAL(started()), worker, SLOT(working()));
    connect(worker, SIGNAL(workingResult(QString)), this, SLOT(WorkUpdate(QString)));
    connect(worker, SIGNAL(workFinished(DirectoryParse, ParseTypes::ParseAction)), workerThread, SLOT(quit()), Qt::DirectConnection);
    connect(worker, SIGNAL(workFinished(DirectoryParse, ParseTypes::ParseAction)), this, SLOT(WorkFinished(DirectoryParse, ParseTypes::ParseAction)));

    workerThread->wait();

    ui->comboGroup->addItem("Select Group");
    ui->comboIndividual->addItem("Select Individual");
    ui->comboEvaluation->addItem("Select Evaluation");

    worker->startWork();
}

ReliabilityDialog::~ReliabilityDialog()
{
    delete ui;
}

void ReliabilityDialog::on_comboGroup_currentIndexChanged(int index)
{
    if (index == 0)
    {
        while (ui->comboIndividual->count() > 1)
        {
            ui->comboIndividual->removeItem(1);
        }
        while (ui->comboEvaluation->count() > 1)
        {
            ui->comboEvaluation->removeItem(1);
        }

        return;
    }

    workerThread = new QThread();

    mCurrentDirectory.WorkingDirectory = mWorkingDirectory;
    mCurrentDirectory.CurrentGroup = ui->comboGroup->currentText();
    mCurrentDirectory.CurrentIndividual = "";
    mCurrentDirectory.CurrentEvaluation = "";
    mCurrentDirectory.CurrentCondition = "";
    mCurrentDirectory.CurrentKeySet = "";

    worker = new DirectorySearcher(mCurrentDirectory);

    worker->moveToThread(workerThread);

    connect(worker, SIGNAL(workStarted()), workerThread, SLOT(start()));
    connect(workerThread, SIGNAL(started()), worker, SLOT(working()));
    connect(worker, SIGNAL(workingResult(QString)), this, SLOT(WorkUpdate(QString)));
    connect(worker, SIGNAL(workFinished(DirectoryParse, ParseTypes::ParseAction)), workerThread, SLOT(quit()), Qt::DirectConnection);
    connect(worker, SIGNAL(workFinished(DirectoryParse, ParseTypes::ParseAction)), this, SLOT(WorkFinished(DirectoryParse, ParseTypes::ParseAction)));

    workerThread->wait();
    worker->startWork();
}

void ReliabilityDialog::on_comboIndividual_currentIndexChanged(int index)
{
    if (index == 0)
    {
        while (ui->comboEvaluation->count() > 1)
        {
            ui->comboEvaluation->removeItem(1);
        }

        return;
    }

    workerThread = new QThread();

    mCurrentDirectory.WorkingDirectory = mWorkingDirectory;
    mCurrentDirectory.CurrentGroup = ui->comboGroup->currentText();
    mCurrentDirectory.CurrentIndividual = ui->comboIndividual->currentText();
    mCurrentDirectory.CurrentEvaluation = "";
    mCurrentDirectory.CurrentCondition = "";
    mCurrentDirectory.CurrentKeySet = "";

    worker = new DirectorySearcher(mCurrentDirectory);

    worker->moveToThread(workerThread);

    connect(worker, SIGNAL(workStarted()), workerThread, SLOT(start()));
    connect(workerThread, SIGNAL(started()), worker, SLOT(working()));
    connect(worker, SIGNAL(workingResult(QString)), this, SLOT(WorkUpdate(QString)));
    connect(worker, SIGNAL(workFinished(DirectoryParse, ParseTypes::ParseAction)), workerThread, SLOT(quit()), Qt::DirectConnection);
    connect(worker, SIGNAL(workFinished(DirectoryParse, ParseTypes::ParseAction)), this, SLOT(WorkFinished(DirectoryParse, ParseTypes::ParseAction)));

    workerThread->wait();
    worker->startWork();
}

void ReliabilityDialog::on_comboEvaluation_currentIndexChanged(int index)
{
    if (index == 0)
    {
        return;
    }

    workerThread = new QThread();

    mCurrentDirectory.WorkingDirectory = mWorkingDirectory;
    mCurrentDirectory.CurrentGroup = ui->comboGroup->currentText();
    mCurrentDirectory.CurrentIndividual = ui->comboIndividual->currentText();
    mCurrentDirectory.CurrentEvaluation = ui->comboEvaluation->currentText();
    mCurrentDirectory.CurrentCondition = "";
    mCurrentDirectory.CurrentKeySet = "";

    QString mFilePath = FileTools::pathAppend(mWorkingDirectory, ui->comboGroup->currentText());
    mFilePath = FileTools::pathAppend(mFilePath, ui->comboIndividual->currentText());
    mFilePath = FileTools::pathAppend(mFilePath, ui->comboEvaluation->currentText());

    ReliabilityObjects.clear();

    QDirIterator iterator(mFilePath,
                          QStringList() << "*.json",
                          QDir::Files,
                          QDirIterator::Subdirectories);

    while (iterator.hasNext())
    {
        QString mFileName = iterator.next();

        if (mFileName.contains(".json", Qt::CaseInsensitive))
        {
            qDebug() << mFileName;

            QFile mSession(mFileName);

            if (mSession.exists())
            {
                if (mSession.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    QString sessionData = mSession.readAll();
                    mSession.close();

                    QJsonDocument loadSession = QJsonDocument::fromJson(sessionData.toUtf8());
                    QJsonObject sessionObject = loadSession.object();

                    ReliabilityParse mReliObj;

                    mReliObj.SessionNumber = sessionObject["Session"].toInt();
                    mReliObj.Collector = sessionObject["Collector"].toString();
                    mReliObj.Condition = sessionObject["Condition"].toString();
                    mReliObj.Reli = !(sessionObject["Role"].toString().contains("Primary", Qt::CaseInsensitive));

                    ReliabilityObjects.append(mReliObj);
                }
            }
        }
    }

    ui->tableWidgetReli->setRowCount(0);

    QString mPrimary, mReli;

    for (int i(0); i<ReliabilityObjects.count(); i++)
    {
        if (!ReliabilityObjects.at(i).Reli)
        {
            ui->tableWidgetReli->insertRow(ui->tableWidgetReli->rowCount());

            ui->tableWidgetReli->setItem(ui->tableWidgetReli->rowCount() - 1, 0, new QTableWidgetItem(QString::number(ReliabilityObjects.at(i).SessionNumber)));
            ui->tableWidgetReli->setItem(ui->tableWidgetReli->rowCount() - 1, 1, new QTableWidgetItem(ReliabilityObjects.at(i).Condition));
            ui->tableWidgetReli->setItem(ui->tableWidgetReli->rowCount() - 1, 2, new QTableWidgetItem(ReliabilityObjects.at(i).Collector));
            ui->tableWidgetReli->setItem(ui->tableWidgetReli->rowCount() - 1, 3, new QTableWidgetItem("---"));

            //ui->tableWidgetReli->setItem(ui->tableWidgetReli->rowCount() - 1, 2, new QTableWidgetItem(ReliabilityObjects.at(i).Collector));
            //ui->tableWidgetReli->setItem(ui->tableWidgetReli->rowCount() - 1, 3, new QTableWidgetItem(formatPercentage(DurationThree.at(i).second, TimeThree)));

        }
    }
}

void ReliabilityDialog::WorkUpdate(QString update)
{
    qDebug() << "WORK_UPDATE: " << update;
}

void ReliabilityDialog::WorkFinished(DirectoryParse finalResult, ParseTypes::ParseAction action)
{
    if (action == ParseTypes::ParseAction::Group)
    {
        while (ui->comboGroup->count() > 1)
        {
            ui->comboGroup->removeItem(1);
        }
        while (ui->comboIndividual->count() > 1)
        {
            ui->comboIndividual->removeItem(1);
        }
        while (ui->comboEvaluation->count() > 1)
        {
            ui->comboEvaluation->removeItem(1);
        }

        for (int i(0); i<finalResult.Groups.count(); i++)
        {
            ui->comboGroup->addItem(finalResult.Groups.at(i));
        }
    }
    else if (action == ParseTypes::ParseAction::Individual)
    {
        while (ui->comboIndividual->count() > 1)
        {
            ui->comboIndividual->removeItem(1);
        }
        while (ui->comboEvaluation->count() > 1)
        {
            ui->comboEvaluation->removeItem(1);
        }

        for (int i(0); i<finalResult.Individuals.count(); i++)
        {
            ui->comboIndividual->addItem(finalResult.Individuals.at(i));
        }
    }
    else if (action == ParseTypes::ParseAction::Evaluation)
    {
        while (ui->comboEvaluation->count() > 1)
        {
            ui->comboEvaluation->removeItem(1);
        }

        for (int i(0); i<finalResult.Evaluations.count(); i++)
        {
            ui->comboEvaluation->addItem(finalResult.Evaluations.at(i));
        }
    }
}
