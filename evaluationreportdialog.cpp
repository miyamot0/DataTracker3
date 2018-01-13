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

#include "evaluationreportdialog.h"
#include "ui_evaluationreportdialog.h"

#include <QThread>
#include <QDebug>

#include "windowtools.h"
#include "filetools.h"

EvaluationReportDialog::EvaluationReportDialog(QString mCurrentWorkingDirectory, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EvaluationReportDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("Evaluation Report Generator"));

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

    ui->comboBoxGroup->addItem("Select Group");
    ui->comboBoxIndividual->addItem("Select Individual");
    ui->comboBoxEvaluation->addItem("Select Evaluation");

    worker->startWork();

    connect(&fadeTimer, SIGNAL(timeout()), this, SLOT(ClearStatus()));

    WindowTools::SetDialogFixedMaximize(this);
}

EvaluationReportDialog::~EvaluationReportDialog()
{
    delete ui;
}

/**
 * @brief EvaluationReportDialog::on_comboBoxGroup_currentIndexChanged
 * @param index
 */
void EvaluationReportDialog::on_comboBoxGroup_currentIndexChanged(int index)
{
    if (index == 0)
    {
        while (ui->comboBoxIndividual->count() > 1)
        {
            ui->comboBoxIndividual->removeItem(1);
        }
        while (ui->comboBoxEvaluation->count() > 1)
        {
            ui->comboBoxEvaluation->removeItem(1);
        }

        ui->comboBoxIndividual->setEnabled(false);
        ui->comboBoxEvaluation->setEnabled(false);

        ui->tableWidget->setRowCount(0);

        return;
    }
    else
    {
        workerThread = new QThread();

        mCurrentDirectory.WorkingDirectory = mWorkingDirectory;
        mCurrentDirectory.CurrentGroup = ui->comboBoxGroup->currentText();
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

        ui->comboBoxIndividual->setEnabled(true);
        ui->comboBoxEvaluation->setEnabled(false);
    }
}

/**
 * @brief EvaluationReportDialog::on_comboBoxIndividual_currentIndexChanged
 * @param index
 */
void EvaluationReportDialog::on_comboBoxIndividual_currentIndexChanged(int index)
{
    if (index == 0)
    {
        while (ui->comboBoxEvaluation->count() > 1)
        {
            ui->comboBoxEvaluation->removeItem(1);
        }

        ui->tableWidget->setRowCount(0);

        ui->comboBoxEvaluation->setEnabled(false);

        return;
    }
    else
    {
        workerThread = new QThread();

        mCurrentDirectory.WorkingDirectory = mWorkingDirectory;
        mCurrentDirectory.CurrentGroup = ui->comboBoxGroup->currentText();
        mCurrentDirectory.CurrentIndividual = ui->comboBoxIndividual->currentText();
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

        ui->comboBoxEvaluation->setEnabled(true);
    }
}

/**
 * @brief EvaluationReportDialog::on_comboBoxEvaluation_currentIndexChanged
 * @param index
 */
void EvaluationReportDialog::on_comboBoxEvaluation_currentIndexChanged(int index)
{
    if (index == 0)
    {
        ui->tableWidget->setRowCount(0);

        return;
    }

    workerThread = new QThread();

    mCurrentDirectory.WorkingDirectory = mWorkingDirectory;
    mCurrentDirectory.CurrentGroup = ui->comboBoxGroup->currentText();
    mCurrentDirectory.CurrentIndividual = ui->comboBoxIndividual->currentText();
    mCurrentDirectory.CurrentEvaluation = ui->comboBoxEvaluation->currentText();
    mCurrentDirectory.CurrentCondition = "";
    mCurrentDirectory.CurrentKeySet = "";

    QString mFilePath = FileTools::pathAppend(mWorkingDirectory, ui->comboBoxGroup->currentText());
    mFilePath = FileTools::pathAppend(mFilePath, ui->comboBoxIndividual->currentText());
    mFilePath = FileTools::pathAppend(mFilePath, ui->comboBoxEvaluation->currentText());

    PrimaryReliabilityObjects.clear();

    QDirIterator iterator(mFilePath,
                          QStringList() << "*.json",
                          QDir::Files,
                          QDirIterator::Subdirectories);

    while (iterator.hasNext())
    {
        QString mFileName = iterator.next();

        if (mFileName.contains(".json", Qt::CaseInsensitive))
        {
            QFile mSession(mFileName);

            if (mSession.exists())
            {
                if (mSession.open(QIODevice::ReadOnly | QIODevice::Text))
                {
                    QString sessionData = mSession.readAll();
                    mSession.close();

                    QJsonDocument loadSession = QJsonDocument::fromJson(sessionData.toUtf8());

                    if (!loadSession.isNull())
                    {
                        QJsonObject sessionObject = loadSession.object();

                        if((sessionObject["Role"].toString().contains("Primary", Qt::CaseInsensitive)))
                        {
                            ReliabilityParse mReliObj;
                            mReliObj.SessionNumber = sessionObject["Session"].toInt();
                            mReliObj.Collector = sessionObject["Collector"].toString();
                            mReliObj.Condition = sessionObject["Condition"].toString();
                            mReliObj.SecondaryObserver = QString("---");
                            mReliObj.PrimaryFilePath = mFileName;
                            mReliObj.Reli = false;
                            mReliObj.CanScoreAsReli = false;

                            PrimaryReliabilityObjects.append(mReliObj);
                        }
                        else
                        {
                            ReliabilityParse mReliObj;
                            mReliObj.SessionNumber = sessionObject["Session"].toInt();
                            mReliObj.Collector = sessionObject["Collector"].toString();
                            mReliObj.Condition = sessionObject["Condition"].toString();
                            mReliObj.PrimaryFilePath = mFileName;
                            mReliObj.Reli = true;
                            mReliObj.CanScoreAsReli = false;

                            SecondaryReliabilityObjects.append(mReliObj);
                        }
                    }
                }
            }
        }
    }

    for (int i(0); i < PrimaryReliabilityObjects.count(); i++)
    {
        for (int j(0); j < SecondaryReliabilityObjects.count(); j++)
        {
            if (PrimaryReliabilityObjects[i].SessionNumber == SecondaryReliabilityObjects[j].SessionNumber &&
                    PrimaryReliabilityObjects[i].Condition == SecondaryReliabilityObjects[j].Condition)
            {
                PrimaryReliabilityObjects[i].SecondaryObserver = SecondaryReliabilityObjects[j].Collector;
                PrimaryReliabilityObjects[i].ReliFilePath = SecondaryReliabilityObjects[j].PrimaryFilePath;
                PrimaryReliabilityObjects[i].CanScoreAsReli = true;
            }
        }
    }

    ui->tableWidget->setRowCount(0);

    std::sort(PrimaryReliabilityObjects.begin(), PrimaryReliabilityObjects.end(),
          [](const ReliabilityParse a, const ReliabilityParse b) -> bool {return a.SessionNumber < b.SessionNumber;});

    for (int i(0); i<PrimaryReliabilityObjects.count(); i++)
    {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());

        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, new QTableWidgetItem(QString::number(PrimaryReliabilityObjects.at(i).SessionNumber)));
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, new QTableWidgetItem(PrimaryReliabilityObjects.at(i).Condition));
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 2,
                                 new QTableWidgetItem(QString("%1 (%2)")
                                                      .arg(PrimaryReliabilityObjects.at(i).Collector)
                                                      .arg(PrimaryReliabilityObjects.at(i).CanScoreAsReli ? "Yes" : "No")));
    }
}

/**
 * @brief EvaluationReportDialog::WorkUpdate
 * @param update
 */
void EvaluationReportDialog::WorkUpdate(QString update)
{
    //qDebug() << "WORK_UPDATE: " << update;
}

/**
 * @brief EvaluationReportDialog::WorkFinished
 * @param finalResult
 * @param action
 */
void EvaluationReportDialog::WorkFinished(DirectoryParse finalResult, ParseTypes::ParseAction action)
{
    if (action == ParseTypes::ParseAction::Group)
    {
        while (ui->comboBoxGroup->count() > 1)
        {
            ui->comboBoxGroup->removeItem(1);
        }
        while (ui->comboBoxIndividual->count() > 1)
        {
            ui->comboBoxIndividual->removeItem(1);
        }
        while (ui->comboBoxEvaluation->count() > 1)
        {
            ui->comboBoxEvaluation->removeItem(1);
        }

        ui->tableWidget->setRowCount(0);

        for (int i(0); i<finalResult.Groups.count(); i++)
        {
            ui->comboBoxGroup->addItem(finalResult.Groups.at(i));
        }
    }
    else if (action == ParseTypes::ParseAction::Individual)
    {
        while (ui->comboBoxIndividual->count() > 1)
        {
            ui->comboBoxIndividual->removeItem(1);
        }
        while (ui->comboBoxEvaluation->count() > 1)
        {
            ui->comboBoxEvaluation->removeItem(1);
        }

        ui->tableWidget->setRowCount(0);

        for (int i(0); i<finalResult.Individuals.count(); i++)
        {
            ui->comboBoxIndividual->addItem(finalResult.Individuals.at(i));
        }
    }
    else if (action == ParseTypes::ParseAction::Evaluation)
    {
        while (ui->comboBoxEvaluation->count() > 1)
        {
            ui->comboBoxEvaluation->removeItem(1);
        }

        ui->tableWidget->setRowCount(0);

        for (int i(0); i<finalResult.Evaluations.count(); i++)
        {
            ui->comboBoxEvaluation->addItem(finalResult.Evaluations.at(i));
        }
    }
}

/**
 * @brief EvaluationReportDialog::ClearStatus
 */
void EvaluationReportDialog::ClearStatus()
{
    ui->labelStatus->setText("");
}

/**
 * @brief EvaluationReportDialog::on_pushButton_clicked
 */
void EvaluationReportDialog::on_pushButton_clicked()
{
    if (PrimaryReliabilityObjects.count() < 1)
    {
        return;
    }

    QJsonArray keyArray;

    QStringList frequencyKeyList, durationKeyList;

    // Get keys, add lines
    for (int i(0); i<PrimaryReliabilityObjects.count(); i++)
    {
        if (FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(i).PrimaryFilePath, &json))
        {
            keyArray = json["FrequencyKeys"].toArray();
            foreach (const QJsonValue collector, keyArray) {
                QJsonObject mObj = collector.toObject();

                if (!frequencyKeyList.contains(mObj["Description"].toString()))
                {
                    frequencyKeyList.append(mObj["Description"].toString());
                }
            }

            keyArray = json["DurationKeys"].toArray();
            foreach (const QJsonValue collector, keyArray) {
                QJsonObject mObj = collector.toObject();

                if (!durationKeyList.contains(mObj["Description"].toString()))
                {
                    durationKeyList.append(mObj["Description"].toString());
                }
            }
        }
    }

    bool writeReport = FileTools::WriteEvaluationReport(&PrimaryReliabilityObjects,
                                                        mWorkingDirectory,
                                                        ui->comboBoxGroup->currentText(),
                                                        ui->comboBoxIndividual->currentText(),
                                                        ui->comboBoxEvaluation->currentText(),
                                                        &frequencyKeyList,
                                                        &durationKeyList,
                                                        &ReportResults);

    ui->labelStatus->setText(writeReport ? "Report saved successfully." : "Report write failed.");

    fadeTimer.setSingleShot(true);
    fadeTimer.start(3000);

    mDisplayDialog.InsertData(&ReportResults);
    mDisplayDialog.exec();
}
