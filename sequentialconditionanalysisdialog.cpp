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

#include "sequentialconditionanalysisdialog.h"
#include "ui_sequentialconditionanalysisdialog.h"
#include "windowtools.h"
#include "parsetypes.h"
#include "filetools.h"
#include "probabilitytools.h"

SequentialConditionAnalysisDialog::SequentialConditionAnalysisDialog(QString mCurrentWorkingDirectory, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SequentialConditionAnalysisDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("Condition Sequential Analysis"));

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

    WindowTools::SetDialogFixedMaximize(this);
}

SequentialConditionAnalysisDialog::~SequentialConditionAnalysisDialog()
{
    delete ui;
}

/**
 * @brief EvaluationReportDialog::WorkUpdate
 * @param update
 */
void SequentialConditionAnalysisDialog::WorkUpdate(QString update)
{
    qDebug() << "WORK_UPDATE: " << update;
}

/**
 * @brief EvaluationReportDialog::WorkFinished
 * @param finalResult
 * @param action
 */
void SequentialConditionAnalysisDialog::WorkFinished(DirectoryParse finalResult, ParseTypes::ParseAction action)
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
    else if (action == ParseTypes::ParseAction::Condition)
    {
        while (ui->comboBoxCondition->count() > 1)
        {
            ui->comboBoxCondition->removeItem(1);
        }

        ui->tableWidget->setRowCount(0);

        for (int i(0); i<finalResult.Conditions.count(); i++)
        {
            ui->comboBoxCondition->addItem(finalResult.Conditions.at(i));
        }
    }
}

/**
 * @brief SequentialConditionAnalysisDialog::on_comboBoxGroup_currentIndexChanged
 * @param index
 */
void SequentialConditionAnalysisDialog::on_comboBoxGroup_currentIndexChanged(int index)
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
        ui->comboBoxCondition->setEnabled(false);
        ui->comboBoxWindowSize->setEnabled(false);
        ui->comboBoxMetric->setEnabled(false);

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
        ui->comboBoxCondition->setEnabled(false);
        ui->comboBoxWindowSize->setEnabled(false);
        ui->comboBoxMetric->setEnabled(false);
    }
}

/**
 * @brief SequentialConditionAnalysisDialog::on_comboBoxIndividual_currentIndexChanged
 * @param index
 */
void SequentialConditionAnalysisDialog::on_comboBoxIndividual_currentIndexChanged(int index)
{
    if (index == 0)
    {
        while (ui->comboBoxEvaluation->count() > 1)
        {
            ui->comboBoxEvaluation->removeItem(1);
        }

        ui->tableWidget->setRowCount(0);

        ui->comboBoxEvaluation->setEnabled(false);
        ui->comboBoxCondition->setEnabled(false);
        ui->comboBoxWindowSize->setEnabled(false);
        ui->comboBoxMetric->setEnabled(false);

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
        ui->comboBoxCondition->setEnabled(false);
        ui->comboBoxWindowSize->setEnabled(false);
        ui->comboBoxMetric->setEnabled(false);
    }
}

/**
 * @brief SequentialConditionAnalysisDialog::on_comboBoxEvaluation_currentIndexChanged
 * @param index
 */
void SequentialConditionAnalysisDialog::on_comboBoxEvaluation_currentIndexChanged(int index)
{
    if (index == 0)
    {
        ui->tableWidget->setRowCount(0);

        ui->comboBoxCondition->setEnabled(false);
        ui->comboBoxWindowSize->setEnabled(false);
        ui->comboBoxMetric->setEnabled(false);

        return;
    }

    workerThread = new QThread();

    mCurrentDirectory.WorkingDirectory = mWorkingDirectory;
    mCurrentDirectory.CurrentGroup = ui->comboBoxGroup->currentText();
    mCurrentDirectory.CurrentIndividual = ui->comboBoxIndividual->currentText();
    mCurrentDirectory.CurrentEvaluation = ui->comboBoxEvaluation->currentText();
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

    ui->comboBoxCondition->setEnabled(true);
    ui->comboBoxWindowSize->setEnabled(false);
    ui->comboBoxMetric->setEnabled(false);
}

/**
 * @brief SequentialConditionAnalysisDialog::on_comboBoxCondition_currentIndexChanged
 * @param index
 */
void SequentialConditionAnalysisDialog::on_comboBoxCondition_currentIndexChanged(int index)
{
    if (index == 0)
    {
        ui->tableWidget->setRowCount(0);
        ui->comboBoxWindowSize->setEnabled(false);
        ui->comboBoxMetric->setEnabled(false);

        return;
    }


    ui->comboBoxWindowSize->setEnabled(true);
    ui->comboBoxMetric->setEnabled(true);

    QString mFilePath = FileTools::pathAppend(mWorkingDirectory, ui->comboBoxGroup->currentText());
    mFilePath = FileTools::pathAppend(mFilePath, ui->comboBoxIndividual->currentText());
    mFilePath = FileTools::pathAppend(mFilePath, ui->comboBoxEvaluation->currentText());
    mFilePath = FileTools::pathAppend(mFilePath, ui->comboBoxCondition->currentText());

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
                                 new QTableWidgetItem(QString("%1").arg(PrimaryReliabilityObjects.at(i).Collector)));
    }

}

/**
 * @brief SequentialAnalysisDialog::GetKeys
 */
void SequentialConditionAnalysisDialog::GetKeys()
{
    QJsonArray keyArray;

    if (PrimaryReliabilityObjects.count() > 0 && FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(0).PrimaryFilePath, &json))
    {
        keyList.clear();
        mScoreKey.clear();

        // Get all keys
        for (int i(0); i<PrimaryReliabilityObjects.count(); i++)
        {
            if (FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(i).PrimaryFilePath, &json))
            {
                keyArray = json["FrequencyKeys"].toArray();
                foreach (const QJsonValue collector, keyArray) {
                    QJsonObject mObj = collector.toObject();

                    if (!keyList.contains((mObj["Description"].toString())))
                    {
                        keyList.append((mObj["Description"].toString()));
                        mScoreKey.append(QPair<QString, int>(mObj["Description"].toString(), 1));
                    }
                }

                keyArray = json["DurationKeys"].toArray();
                foreach (const QJsonValue collector, keyArray) {
                    QJsonObject mObj = collector.toObject();

                    if (!keyList.contains((mObj["Description"].toString())))
                    {
                        keyList.append((mObj["Description"].toString()));
                        mScoreKey.append(QPair<QString, int>(mObj["Description"].toString(), 2));
                    }
                }
            }
        }
    }
}

/**
 * @brief SequentialConditionAnalysisDialog::on_comboBoxWindowSize_currentIndexChanged
 * @param index
 */
void SequentialConditionAnalysisDialog::on_comboBoxWindowSize_currentIndexChanged(int *)
{
    if (ui->comboBoxWindowSize->currentIndex() > 0 && ui->comboBoxMetric->currentIndex() > 0)
    {
        on_pushButton_clicked();
    }
}

/**
 * @brief SequentialConditionAnalysisDialog::ChartYule
 */
void SequentialConditionAnalysisDialog::ChartMetric()
{
    int windowSpan = GetWindowSpan();

    if (windowSpan == -1)
    {
        return;
    }

    ProbabilityTools::ProbabilityType analysisType = GetMetric();

    if (analysisType == ProbabilityTools::Blank)
    {
        return;
    }

    QString mAnalysis = (analysisType == ProbabilityTools::OddsRatio ? "Odds Ratio" :
                                                                       analysisType == ProbabilityTools::YulesQ ? "Yules Q" : "Operant Contingency Value");

    ui->labelTitle->setText(QString("<html><head/><body><p align='center'><span style='font-size:10pt; font-weight:600;'>%1 (Condition: %2)</span></p></body></html>")
                            .arg(mAnalysis)
                            .arg(ui->comboBoxCondition->currentText()));

    QList<QList<QPair<LagCoding, LagCoding>>> tableConstruction;

    for (int i(0); i<keyList.count(); i++)
    {
        QList<QPair<LagCoding, LagCoding>> temp;
        temp.clear();

        for (int j(0); j<keyList.count(); j++)
        {
            temp.append(QPair<LagCoding, LagCoding>());
        }

        tableConstruction.append(temp);
    }

    QTableWidgetItem * tempItem;

    QList<QStringList> mResults;
    mResults.clear();

    ProbabilityTools::FillMetaContingencyTable(&PrimaryReliabilityObjects,
                                               &mResults,
                                               keyList,
                                               mScoreKey,
                                               tableConstruction,
                                               analysisType,
                                               1, 4);

    ui->tableWidgetOutputs->clearContents();
    ui->tableWidgetOutputs->setRowCount(0);
    ui->tableWidgetOutputs->setColumnCount(0);

    // Create columns
    for (int i(0); i<mScoreKey.count(); i++)
    {
        ui->tableWidgetOutputs->insertColumn(ui->tableWidgetOutputs->columnCount());
    }

    // Create rows
    for (int i(0); i<mScoreKey.count(); i++)
    {
        ui->tableWidgetOutputs->insertRow(ui->tableWidgetOutputs->rowCount());

        for (int j(0); j<mScoreKey.count(); j++)
        {
            tempItem = new QTableWidgetItem(mResults.at(i).at(j));
            tempItem->setToolTip(QString("%1 followed by %2").arg(mScoreKey.at(i).first).arg(mScoreKey.at(j).first));

            // Remove for now
            //tempItem->setBackgroundColor(GetColorMapping(mResults.at(i).at(j)));
            ui->tableWidgetOutputs->setItem(i, j, tempItem);
            ui->tableWidgetOutputs->setVerticalHeaderItem(j,
                                                          new QTableWidgetItem(QString("%1 -> ...").arg(mScoreKey.at(j).first)));
        }

        ui->tableWidgetOutputs->setHorizontalHeaderItem(i,
                                                        new QTableWidgetItem(QString("... -> %1").arg(mScoreKey.at(i).first)));
    }
}

/**
 * @brief SequentialAnalysisDialog::GetColorMapping
 * @param value
 * @return
 */
QColor SequentialConditionAnalysisDialog::GetColorMapping(QString value)
{
    bool isNumber;

    double num = value.toDouble(&isNumber);

    if (isNumber)
    {
        if (num < -0.7)
        {
            // Dark orange
            return QColor(227,74,51);
        }
        else if (num >= -0.7 && num < -0.5)
        {
            // Mid orange
            return QColor(253,187,132);
        }
        else if (num >= -0.5 && num <= -0.2)
        {
            // Light orange
            return QColor(254,232,200);
        }
        else if (num > -0.2 && num < 0.2)
        {
            // No color
            return QColor(Qt::transparent);
        }
        else if (num >= 0.2 && num <= 0.5)
        {
            // Light green
            return QColor(229,245,224);
        }
        else if (num > 0.5 && num <= 0.7)
        {
            // Mid green
            return QColor(161, 217, 155);
        }
        else if (num > 0.7)
        {
            // Dark green
            return QColor(49, 163, 84);
        }
        else
        {
            return QColor(Qt::transparent);
        }
    }
    else
    {
        return QColor(Qt::transparent);
    }
}

/**
 * @brief SequentialAnalysisDialog::GetWindowSpan
 * @return
 */
int SequentialConditionAnalysisDialog::GetWindowSpan()
{
    int seconds = -1;

    if (ui->comboBoxWindowSize->currentIndex() == 1)
    {
        seconds = 2;
    }
    else if (ui->comboBoxWindowSize->currentIndex() == 2)
    {
        seconds = 3;
    }
    else if (ui->comboBoxWindowSize->currentIndex() == 3)
    {
        seconds = 4;
    }
    else if (ui->comboBoxWindowSize->currentIndex() == 4)
    {
        seconds = 5;
    }
    else if (ui->comboBoxWindowSize->currentIndex() == 5)
    {
        seconds = 10;
    }
    else if (ui->comboBoxWindowSize->currentIndex() == 6)
    {
        seconds = 15;
    }
    else if (ui->comboBoxWindowSize->currentIndex() == 7)
    {
        seconds = 30;
    }

    return seconds;
}

/**
 * @brief SequentialConditionAnalysisDialog::GetMetric
 * @return
 */
ProbabilityTools::ProbabilityType SequentialConditionAnalysisDialog::GetMetric()
{
    if (ui->comboBoxMetric->currentIndex() == 1)
    {
        return ProbabilityTools::YulesQ;
    }
    else if (ui->comboBoxMetric->currentIndex() == 2)
    {
        return ProbabilityTools::OperantContingencyValue;
    }
    else if (ui->comboBoxMetric->currentIndex() == 3)
    {
        return ProbabilityTools::OddsRatio;
    }
    else
    {
        return ProbabilityTools::Blank;
    }
}

/**
 * @brief SequentialConditionAnalysisDialog::on_pushButton_clicked
 */
void SequentialConditionAnalysisDialog::on_pushButton_clicked()
{
    GetKeys();

    mSeriesSelect.setWindowTitle(tr("Select Keys to Analyze"));
    mSeriesSelect.AddOptions(keyList);

    if (mSeriesSelect.exec() != QDialog::Rejected)
    {
        keyShowList = mSeriesSelect.GetBoolList();

        for (int i = keyShowList.count() - 1; i >= 0; i--)
        {
            if (keyShowList[i] == false)
            {
                mScoreKey.removeAt(i);
            }
        }

        ChartMetric();
    }
}

/**
 * @brief SequentialConditionAnalysisDialog::on_comboBoxMetric_currentIndexChanged
 * @param index
 */
void SequentialConditionAnalysisDialog::on_comboBoxMetric_currentIndexChanged(int *)
{
    if (ui->comboBoxWindowSize->currentIndex() > 0 && ui->comboBoxMetric->currentIndex() > 0)
    {
        on_pushButton_clicked();
    }
}
