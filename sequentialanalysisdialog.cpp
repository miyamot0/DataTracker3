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

#include "sequentialanalysisdialog.h"
#include "ui_sequentialanalysisdialog.h"

#include "windowtools.h"

#include <QDirIterator>
#include <QFile>
#include <QJsonDocument>

#include "filetools.h"

SequentialAnalysisDialog::SequentialAnalysisDialog(QString mCurrentWorkingDirectory, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SequentialAnalysisDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("Sequence Analyzer"));

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

SequentialAnalysisDialog::~SequentialAnalysisDialog()
{
    delete ui;
}

/**
 * @brief SequentialAnalysisDialog::on_comboBoxGroup_currentIndexChanged
 * @param index
 */
void SequentialAnalysisDialog::on_comboBoxGroup_currentIndexChanged(int index)
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

        ui->tableWidget->setRowCount(0);

        ui->comboBoxIndividual->setEnabled(false);
        ui->comboBoxEvaluation->setEnabled(false);

        keyList.clear();
        ui->tableWidgetOutputs->clear();

        ui->comboBoxIndividual->setEnabled(false);
        ui->comboBoxEvaluation->setEnabled(false);
        ui->comboBoxAnalysis->setEnabled(false);
        ui->comboBoxWindow->setEnabled(false);
        ui->labelTitle->setText(QString("<html><head/><body><p align='center'><span style='font-size:10pt; font-weight:600;'>Sequential Analyses</span></p></body></html>"));

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
        ui->comboBoxAnalysis->setEnabled(false);
        ui->comboBoxWindow->setEnabled(false);
    }
}

/**
 * @brief SequentialAnalysisDialog::on_comboBoxIndividual_currentIndexChanged
 * @param index
 */
void SequentialAnalysisDialog::on_comboBoxIndividual_currentIndexChanged(int index)
{

    if (index == 0)
    {
        while (ui->comboBoxEvaluation->count() > 1)
        {
            ui->comboBoxEvaluation->removeItem(1);
        }

        ui->tableWidget->setRowCount(0);

        ui->comboBoxEvaluation->setEnabled(false);

        keyList.clear();
        ui->tableWidgetOutputs->clear();
        ui->comboBoxEvaluation->setEnabled(false);
        ui->comboBoxAnalysis->setEnabled(false);
        ui->comboBoxWindow->setEnabled(false);
        ui->labelTitle->setText(QString("<html><head/><body><p align='center'><span style='font-size:10pt; font-weight:600;'>Sequential Analyses</span></p></body></html>"));

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
        ui->comboBoxAnalysis->setEnabled(false);
        ui->comboBoxWindow->setEnabled(false);
    }
}

/**
 * @brief SequentialAnalysisDialog::on_comboBoxEvaluation_currentIndexChanged
 * @param index
 */
void SequentialAnalysisDialog::on_comboBoxEvaluation_currentIndexChanged(int index)
{
    if (index == 0)
    {
        ui->tableWidget->setRowCount(0);

        keyList.clear();
        ui->tableWidgetOutputs->clear();
        ui->comboBoxAnalysis->setEnabled(false);
        ui->comboBoxWindow->setEnabled(false);
        ui->labelTitle->setText(QString("<html><head/><body><p align='center'><span style='font-size:10pt; font-weight:600;'>Sequential Analyses</span></p></body></html>"));

        return;
    }
    else
    {
        workerThread = new QThread();

        mCurrentDirectory.WorkingDirectory = mWorkingDirectory;
        mCurrentDirectory.CurrentGroup = ui->comboBoxGroup->currentText();
        mCurrentDirectory.CurrentIndividual = ui->comboBoxIndividual->currentText();
        mCurrentDirectory.CurrentEvaluation = ui->comboBoxEvaluation->currentText();
        mCurrentDirectory.CurrentCondition = "";
        mCurrentDirectory.CurrentKeySet = "";

        ui->comboBoxAnalysis->setEnabled(true);
        ui->comboBoxWindow->setEnabled(false);

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

                                PrimaryReliabilityObjects.append(mReliObj);
                            }
                        }
                    }
                }
            }
        }

        std::sort(PrimaryReliabilityObjects.begin(), PrimaryReliabilityObjects.end(),
              [](const ReliabilityParse a, const ReliabilityParse b) -> bool {return a.SessionNumber < b.SessionNumber;});

        ui->tableWidget->setRowCount(0);

        for (int i(0); i<PrimaryReliabilityObjects.count(); i++)
        {
            ui->tableWidget->insertRow(ui->tableWidget->rowCount());

            ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, new QTableWidgetItem(QString::number(PrimaryReliabilityObjects.at(i).SessionNumber)));
            ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, new QTableWidgetItem(PrimaryReliabilityObjects.at(i).Condition));
            ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 2, new QTableWidgetItem(PrimaryReliabilityObjects.at(i).Collector));
        }
    }
}

/**
 * @brief SequentialAnalysisDialog::on_comboBoxAnalysis_currentIndexChanged
 * @param index
 */
void SequentialAnalysisDialog::on_comboBoxAnalysis_currentIndexChanged(int index)
{
    if (index == 0)
    {
        ui->comboBoxWindow->setCurrentIndex(0);
        ui->comboBoxWindow->setEnabled(false);

        keyList.clear();
        ui->tableWidgetOutputs->clear();
        ui->labelTitle->setText(QString("<html><head/><body><p align='center'><span style='font-size:10pt; font-weight:600;'>Sequential Analyses</span></p></body></html>"));
    }
    else if (index == 1)
    {
        ui->comboBoxWindow->setCurrentIndex(0);
        ui->comboBoxWindow->setEnabled(true);
    }
}

/**
 * @brief SequentialAnalysisDialog::on_comboBoxWindow_currentIndexChanged
 * @param index
 */
void SequentialAnalysisDialog::on_comboBoxWindow_currentIndexChanged(int index)
{
    if (index > 0)
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

            ChartYule(0);
        }
    }
}

/**
 * @brief SessionViewerDialog::WorkUpdate
 * @param update
 */
void SequentialAnalysisDialog::WorkUpdate(QString update)
{
    qDebug() << "WORK_UPDATE: " << update;
}

/**
 * @brief SessionViewerDialog::WorkFinished
 * @param finalResult
 * @param action
 */
void SequentialAnalysisDialog::WorkFinished(DirectoryParse finalResult, ParseTypes::ParseAction action)
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
 * @brief SequentialAnalysisDialog::GetKeys
 */
void SequentialAnalysisDialog::GetKeys()
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
 * @brief SequentialAnalysisDialog::ChartYule
 * @param index
 */
void SequentialAnalysisDialog::ChartYule(int index)
{
    temp = PrimaryReliabilityObjects.at(index);
    result = FileTools::ReadSessionFromJSON(temp.PrimaryFilePath, &json);

    int windowSpan = GetWindowSpan();

    QTableWidgetItem * tempItem;

    if (windowSpan == -1)
    {
        return;
    }

    ui->labelTitle->setText(QString("<html><head/><body><p align='center'><span style='font-size:10pt; font-weight:600;'>Sequential Analyses (Session #: %1)</span></p></body></html>").arg(ui->tableWidget->item(index, 0)->text()));

    QList<QStringList> mResults;
    mResults.clear();

    FileTools::CreateContingencyTables(temp.PrimaryFilePath, mScoreKey, &mResults, 1, 1, windowSpan);

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
            tempItem->setBackgroundColor(GetColorMapping(mResults.at(i).at(j)));
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
QColor SequentialAnalysisDialog::GetColorMapping(QString value)
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
int SequentialAnalysisDialog::GetWindowSpan()
{
    int seconds = -1;

    if (ui->comboBoxWindow->currentIndex() == 1)
    {
        seconds = 2;
    }
    else if (ui->comboBoxWindow->currentIndex() == 2)
    {
        seconds = 3;
    }
    else if (ui->comboBoxWindow->currentIndex() == 3)
    {
        seconds = 4;
    }
    else if (ui->comboBoxWindow->currentIndex() == 4)
    {
        seconds = 5;
    }
    else if (ui->comboBoxWindow->currentIndex() == 5)
    {
        seconds = 10;
    }
    else if (ui->comboBoxWindow->currentIndex() == 6)
    {
        seconds = 15;
    }
    else if (ui->comboBoxWindow->currentIndex() == 7)
    {
        seconds = 30;
    }

    return seconds;
}

/**
 * @brief SequentialAnalysisDialog::on_tableWidget_currentCellChanged
 * @param rowChanged
 */
void SequentialAnalysisDialog::on_tableWidget_currentCellChanged(int rowChanged, int, int, int)
{        
    if (ui->comboBoxAnalysis->currentIndex() == 0 || keyList.count() == 0)
    {
        return;
    }
    else if (ui->comboBoxAnalysis->currentIndex() == 1)
    {
        ChartYule(rowChanged);
    }
}
