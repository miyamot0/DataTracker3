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

#include <QDebug>

#include "evaluationviewerdialog.h"
#include "ui_evaluationviewerdialog.h"

#include "filetools.h"
#include "windowtools.h"

EvaluationViewerDialog::EvaluationViewerDialog(QString mCurrentWorkingDirectory, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EvaluationViewerDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("Evaluation Viewer"));

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

    lineSeries.clear();

    chart = new QChart();
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setBackgroundRoundness(0);
    chart->setTitle("");
    chart->setTitleFont(QFont("Serif", 10, -1, false));
    chart->setTitleBrush(Qt::black);

    axisX = new QValueAxis;
    axisX->applyNiceNumbers();
    axisX->setGridLineColor(Qt::transparent);
    axisX->setTitleText(tr("Seconds"));
    axisX->setTitleFont(QFont("Serif", 10, -1, false));
    axisX->setTitleBrush(Qt::black);
    axisX->setMin(0);
    axisX->setLabelsFont(QFont("Serif", 10, -1, false));
    axisX->setLabelFormat(QString("%.0f"));
    axisX->setLabelsBrush(Qt::black);
    axisX->setLabelsColor(Qt::black);
    axisX->setLinePenColor(Qt::black);
    axisX->setLinePen(QPen(Qt::black));

    axisY = new QValueAxis;
    axisY->applyNiceNumbers();
    axisY->setGridLineColor(Qt::transparent);
    axisY->setTitleText(tr("Behavior Count"));
    axisY->setTitleFont(QFont("Serif", 10, -1, false));
    axisY->setTitleBrush(Qt::black);
    axisY->setLabelsFont(QFont("Serif", 10, -1, false));
    axisY->setLabelsBrush(Qt::black);
    axisY->setLabelsColor(Qt::black);
    axisY->setMin(0);
    axisY->setMax(1);
    axisY->setLinePenColor(Qt::black);
    axisY->setLinePen(QPen(Qt::black));

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    ui->plotLayout->addWidget(chartView);

    axisY->setMin(0);

    WindowTools::SetDialogFixedMaximize(this);
}

EvaluationViewerDialog::~EvaluationViewerDialog()
{
    delete ui;
}

/**
 * @brief EvaluationViewerDialog::WorkUpdate
 * @param update
 */
void EvaluationViewerDialog::WorkUpdate(QString update)
{
    qDebug() << "WORK_UPDATE: " << update;
}

/**
 * @brief EvaluationViewerDialog::WorkFinished
 * @param finalResult
 * @param action
 */
void EvaluationViewerDialog::WorkFinished(DirectoryParse finalResult, ParseTypes::ParseAction action)
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

        ui->comboBoxDimension->setCurrentIndex(0);
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

        ui->comboBoxDimension->setCurrentIndex(0);
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

        ui->comboBoxDimension->setCurrentIndex(0);
    }
}

/**
 * @brief EvaluationViewerDialog::on_comboBoxGroup_currentIndexChanged
 * @param index
 */
void EvaluationViewerDialog::on_comboBoxGroup_currentIndexChanged(int index)
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

        return;
    }

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

    ui->comboBoxDimension->setCurrentIndex(0);
}

/**
 * @brief EvaluationViewerDialog::on_comboBoxIndividual_currentIndexChanged
 * @param index
 */
void EvaluationViewerDialog::on_comboBoxIndividual_currentIndexChanged(int index)
{
    if (index == 0)
    {
        while (ui->comboBoxEvaluation->count() > 1)
        {
            ui->comboBoxEvaluation->removeItem(1);
        }

        ui->tableWidget->setRowCount(0);

        return;
    }

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

    ui->comboBoxDimension->setCurrentIndex(0);
}

/**
 * @brief EvaluationViewerDialog::on_comboBoxEvaluation_currentIndexChanged
 * @param index
 */
void EvaluationViewerDialog::on_comboBoxEvaluation_currentIndexChanged(int index)
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
                    }
                }
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
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 2, new QTableWidgetItem(PrimaryReliabilityObjects.at(i).Collector));
    }

    ui->comboBoxDimension->setCurrentIndex(0);
}

/**
 * @brief EvaluationViewerDialog::on_comboBoxDimension_currentIndexChanged
 * @param index
 */
void EvaluationViewerDialog::on_comboBoxDimension_currentIndexChanged(int index)
{
    if (ui->tableWidget->rowCount() < 1)
    {
        return;
    }

    if (index == 1)
    {
        GetFrequencyKeySets();

        QStringList mFrequencyKeys;

        for (QString mKey : fKeyDesc)
        {
            mFrequencyKeys.append(mKey.split("-")[0]);
        }

        fKeyDesc = mFrequencyKeys.toSet().toList();

        mSeriesSelect.AddOptions(fKeyDesc);
        mSeriesSelect.exec();

        fKeyShow = mSeriesSelect.GetBoolList();

        DrawFrequencyPlot();
    }
    else if (index == 2)
    {
        GetDurationKeySets();

        QStringList mDurationKeys;

        for (QString mKey : dKeyDesc)
        {
            mDurationKeys.append(mKey.split("-")[0]);
        }

        dKeyDesc = mDurationKeys.toSet().toList();

        mSeriesSelect.AddOptions(dKeyDesc);
        mSeriesSelect.exec();

        dKeyShow = mSeriesSelect.GetBoolList();

        DrawDurationPlot();
    }
}

/**
 * @brief EvaluationViewerDialog::GetDurationKeySets
 */
void EvaluationViewerDialog::GetDurationKeySets()
{
    QJsonArray durationArray;

    if (PrimaryReliabilityObjects.count() > 0 && FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(0).PrimaryFilePath, &json))
    {
        dKeyDesc.clear();

        // Get keys, add lines
        for (int i(0); i<PrimaryReliabilityObjects.count(); i++)
        {
            if (FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(i).PrimaryFilePath, &json))
            {
                durationArray = json["DurationKeys"].toArray();
                foreach (const QJsonValue collector, durationArray) {
                    QJsonObject mObj = collector.toObject();

                    if (!dKeyDesc.contains((mObj["Description"].toString() + "-" + json["Condition"].toString())))
                    {
                        dKeyDesc.append((mObj["Description"].toString() + "-" + json["Condition"].toString()));
                    }
                }
            }
        }
    }
}

/**
 * @brief EvaluationViewerDialog::GetFrequencyKeySets
 */
void EvaluationViewerDialog::GetFrequencyKeySets()
{
    QJsonArray frequencyArray;

    if (PrimaryReliabilityObjects.count() > 0 && FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(0).PrimaryFilePath, &json))
    {
        fKeyDesc.clear();

        // Get keys, add lines
        for (int i(0); i<PrimaryReliabilityObjects.count(); i++)
        {
            if (FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(i).PrimaryFilePath, &json))
            {
                frequencyArray = json["FrequencyKeys"].toArray();
                foreach (const QJsonValue collector, frequencyArray) {
                    QJsonObject mObj = collector.toObject();

                    if (!fKeyDesc.contains((mObj["Description"].toString() + "-" + json["Condition"].toString())))
                    {
                        fKeyDesc.append((mObj["Description"].toString() + "-" + json["Condition"].toString()));
                    }
                }
            }
        }
    }
}

/**
 * @brief EvaluationViewerDialog::on_pushButton_clicked
 */
void EvaluationViewerDialog::on_pushButton_clicked()
{
    on_comboBoxDimension_currentIndexChanged(ui->comboBoxDimension->currentIndex());
}

/**
 * @brief EvaluationViewerDialog::DrawDurationPlot
 */
void EvaluationViewerDialog::DrawDurationPlot()
{
    chart->removeAllSeries();
    chart->removeAxis(axisX);
    chart->removeAxis(axisY);

    axisX->setTitleText("Session");
    axisY->setTitleText("Total Seconds Recorded");

    QJsonArray durationArray;

    QString tempName;
    QStringList dKeySeriesNames;

    double max = 1;
    int session = 1;

    if (PrimaryReliabilityObjects.count() > 0 && FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(0).PrimaryFilePath, &json))
    {
        chart->removeAllSeries();
        lineSeries.clear();

        for (int i(0); i<PrimaryReliabilityObjects.count(); i++)
        {
            if (FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(i).PrimaryFilePath, &json))
            {
                durationArray = json["DurationKeys"].toArray();
                foreach (const QJsonValue collector, durationArray) {
                    QJsonObject mObj = collector.toObject();

                    int dIndex = dKeyDesc.indexOf(mObj["Description"].toString());

                    if (dIndex != 1 && dKeyShow[dIndex] == true)
                    {
                        tempName = (mObj["Description"].toString() + "-" + json["Condition"].toString());

                        if (!dKeySeriesNames.contains(tempName))
                        {
                            dKeySeriesNames.append(tempName);

                            lineSeries.append(new QLineSeries);

                            lineSeries[dKeySeriesNames.indexOf(tempName)]->setUseOpenGL(true);
                            lineSeries[dKeySeriesNames.indexOf(tempName)]->setName(tempName);
                            lineSeries[dKeySeriesNames.indexOf(tempName)]->clear();
                            lineSeries[dKeySeriesNames.indexOf(tempName)]->show();

                            chart->addSeries(lineSeries[dKeySeriesNames.indexOf(tempName)]);

                            chart->setAxisX(axisX, lineSeries[dKeySeriesNames.indexOf(tempName)]);
                            chart->setAxisY(axisY, lineSeries[dKeySeriesNames.indexOf(tempName)]);
                        }
                    }
                }

                durationArray = json["DurationOverall"].toArray();
                foreach (const QJsonValue collector, durationArray) {
                    QJsonObject mObj = collector.toObject();

                    int dIndex = dKeyDesc.indexOf(mObj["Key"].toString());

                    if (dIndex != 1 && dKeyShow[dIndex] == true)
                    {
                        tempName = (mObj["Key"].toString() + "-" + json["Condition"].toString());

                        *lineSeries[dKeySeriesNames.indexOf(tempName)] << QPointF(json["Session"].toInt(), ((double)json["Count"].toInt() / 1000));

                        if (((double)json["Count"].toInt() / 1000) > max)
                        {
                            max = ((double)json["Count"].toInt() / 1000);
                        }

                        if (json["Session"].toInt() > session)
                        {
                            session = json["Session"].toInt();
                        }
                    }
                }
            }
        }

        axisX->setMin(0);
        axisX->setMax(session);

        axisY->setMin(0);
        axisY->setMax(max);
    }
}

/**
 * @brief EvaluationViewerDialog::DrawFrequencyPlot
 */
void EvaluationViewerDialog::DrawFrequencyPlot()
{
    chart->removeAllSeries();
    chart->removeAxis(axisX);
    chart->removeAxis(axisY);

    axisX->setTitleText("Session");
    axisY->setTitleText("Rate/Minute");

    QJsonArray frequencyArray;

    QString tempName;
    QStringList fKeySeriesNames;

    double max = 1;
    int session = 1;

    if (PrimaryReliabilityObjects.count() > 0 && FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(0).PrimaryFilePath, &json))
    {
        chart->removeAllSeries();
        lineSeries.clear();

        for (int i(0); i<PrimaryReliabilityObjects.count(); i++)
        {
            if (FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(i).PrimaryFilePath, &json))
            {
                frequencyArray = json["FrequencyKeys"].toArray();
                foreach (const QJsonValue collector, frequencyArray) {
                    QJsonObject mObj = collector.toObject();

                    int fIndex = fKeyDesc.indexOf(mObj["Description"].toString());

                    if (fIndex != 1 && fKeyShow[fIndex] == true)
                    {
                        tempName = (mObj["Description"].toString() + "-" + json["Condition"].toString());

                        if (!fKeySeriesNames.contains(tempName))
                        {
                            fKeySeriesNames.append(tempName);

                            lineSeries.append(new QLineSeries);

                            lineSeries[fKeySeriesNames.indexOf(tempName)]->setUseOpenGL(true);
                            lineSeries[fKeySeriesNames.indexOf(tempName)]->setName(tempName);
                            lineSeries[fKeySeriesNames.indexOf(tempName)]->clear();
                            lineSeries[fKeySeriesNames.indexOf(tempName)]->show();

                            chart->addSeries(lineSeries[fKeySeriesNames.indexOf(tempName)]);

                            chart->setAxisX(axisX, lineSeries[fKeySeriesNames.indexOf(tempName)]);
                            chart->setAxisY(axisY, lineSeries[fKeySeriesNames.indexOf(tempName)]);

                        }
                    }
                }

                frequencyArray = json["FrequencyOverall"].toArray();
                foreach (const QJsonValue collector, frequencyArray) {
                    QJsonObject mObj = collector.toObject();

                    int fIndex = fKeyDesc.indexOf(mObj["Key"].toString());

                    if (fIndex != 1 && fKeyShow[fIndex] == true)
                    {                        
                        tempName = (mObj["Key"].toString() + "-" + json["Condition"].toString());

                        *lineSeries[fKeySeriesNames.indexOf(tempName)] << QPointF((double) json["Session"].toInt(), mObj["Rate"].toString().toDouble());
                        //lineSeries[fKeySeriesNames.indexOf(tempName)]->append((double) json["Session"].toInt(), mObj["Rate"].toString().toDouble());

                        if (mObj["Rate"].toString().toDouble() > max)
                        {
                            max = mObj["Rate"].toString().toDouble();
                        }

                        if (json["Session"].toInt() > session)
                        {
                            session = json["Session"].toInt();
                        }
                    }
                }
            }
        }

        axisX->setMin(0);
        axisX->setMax(session);

        axisY->setMin(0);
        axisY->setMax(max);
    }
}
