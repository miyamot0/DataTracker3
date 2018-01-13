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
    axisX->setLabelFormat(QString("%.1f"));
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
    axisY->setMax(100);
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
    //qDebug() << "WORK_UPDATE: " << update;
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

        ui->comboBoxSchedule->setCurrentIndex(0);
        ui->comboBoxDimension->setCurrentIndex(0);

        ui->tableWidget->setRowCount(0);

        ui->comboBoxIndividual->setEnabled(false);
        ui->comboBoxEvaluation->setEnabled(false);
        ui->comboBoxSchedule->setEnabled(false);
        ui->comboBoxDimension->setEnabled(false);

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

        ui->comboBoxDimension->setCurrentIndex(0);

        ui->comboBoxIndividual->setEnabled(true);
        ui->comboBoxEvaluation->setEnabled(false);
        ui->comboBoxSchedule->setEnabled(false);
        ui->comboBoxDimension->setEnabled(false);
    }
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

        ui->comboBoxSchedule->setCurrentIndex(0);
        ui->comboBoxDimension->setCurrentIndex(0);

        ui->tableWidget->setRowCount(0);        

        ui->comboBoxEvaluation->setEnabled(false);
        ui->comboBoxSchedule->setEnabled(false);
        ui->comboBoxDimension->setEnabled(false);

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

        ui->comboBoxDimension->setCurrentIndex(0);

        ui->comboBoxEvaluation->setEnabled(true);
        ui->comboBoxSchedule->setEnabled(false);
        ui->comboBoxDimension->setEnabled(false);

    }
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

        ui->comboBoxSchedule->setEnabled(false);
        ui->comboBoxDimension->setEnabled(false);

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

        ui->comboBoxSchedule->setCurrentIndex(0);
        ui->comboBoxDimension->setCurrentIndex(0);


        ui->comboBoxSchedule->setEnabled(true);
        ui->comboBoxDimension->setEnabled(true);
    }
}

/**
 * @brief EvaluationViewerDialog::on_comboBoxSchedule_currentIndexChanged
 * @param index
 */
void EvaluationViewerDialog::on_comboBoxSchedule_currentIndexChanged(int)
{
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

    if (index == 0)
    {
        DrawBlankPlot();
    }
    if (index == 1)
    {
        GetFrequencyKeySets();

        mSeriesSelect.setWindowTitle(tr("Select keys to include"));
        mSeriesSelect.AddOptions(fKeyDesc);

        if (mSeriesSelect.exec() != QDialog::Rejected)
        {
            fKeyShow = mSeriesSelect.GetBoolList();

            DrawFrequencyPlot();
        }
    }
    else if (index == 2)
    {
        GetDurationKeySets();

        mSeriesSelect.setWindowTitle(tr("Select keys to include"));
        mSeriesSelect.AddOptions(dKeyDesc);

        if (mSeriesSelect.exec() != QDialog::Rejected)
        {
            dKeyShow = mSeriesSelect.GetBoolList();

            DrawDurationPlot();
        }
    }
}

/**
 * @brief EvaluationViewerDialog::GetDurationKeySets
 */
void EvaluationViewerDialog::GetDurationKeySets()
{
    dKeyConds.clear();

    for (int i(0); i<PrimaryReliabilityObjects.count(); i++)
    {
        if (!dKeyConds.contains(PrimaryReliabilityObjects.at(i).Condition))
        {
            dKeyConds.append(PrimaryReliabilityObjects.at(i).Condition);
        }
    }

    if (PrimaryReliabilityObjects.count() > 0 && FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(0).PrimaryFilePath, &json))
    {
        QJsonArray durationArray;

        dKeyDesc.clear();

        // Get keys, add lines
        for (int i(0); i<PrimaryReliabilityObjects.count(); i++)
        {
            if (FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(i).PrimaryFilePath, &json))
            {
                durationArray = json["DurationKeys"].toArray();
                foreach (const QJsonValue collector, durationArray) {
                    QJsonObject mObj = collector.toObject();

                    if (!dKeyDesc.contains(mObj["Description"].toString()))
                    {
                        dKeyDesc.append(mObj["Description"].toString());
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
    fKeyConds.clear();

    for (int i(0); i<PrimaryReliabilityObjects.count(); i++)
    {
        if (!fKeyConds.contains(PrimaryReliabilityObjects.at(i).Condition))
        {
            fKeyConds.append(PrimaryReliabilityObjects.at(i).Condition);
        }
    }

    if (PrimaryReliabilityObjects.count() > 0 && FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(0).PrimaryFilePath, &json))
    {
        QJsonArray durationArray;

        fKeyDesc.clear();

        // Get keys, add lines
        for (int i(0); i<PrimaryReliabilityObjects.count(); i++)
        {
            if (FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(i).PrimaryFilePath, &json))
            {
                durationArray = json["FrequencyKeys"].toArray();
                foreach (const QJsonValue collector, durationArray) {
                    QJsonObject mObj = collector.toObject();

                    if (!fKeyDesc.contains(mObj["Description"].toString()))
                    {
                        fKeyDesc.append(mObj["Description"].toString());
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
 * @brief SessionViewerDialog::DrawBlankPlot
 */
void EvaluationViewerDialog::DrawBlankPlot()
{
    chart->setTitle("");
    chart->removeAllSeries();
    chart->removeAxis(axisX);
    chart->removeAxis(axisY);

    axisX->setTitleText("Counts");
    axisY->setTitleText("Session");
}

/**
 * @brief EvaluationViewerDialog::DrawDurationPlot
 */
void EvaluationViewerDialog::DrawDurationPlot()
{
    chart->removeAllSeries();
    chart->removeAxis(axisX);
    chart->removeAxis(axisY);

    lineSeries.clear();
    pointSeries.clear();

    QString title = QString("Duration Display (");
    bool firstString = true;

    for (int i(0); i<dKeyDesc.count(); i++)
    {
        if (dKeyShow[i] == true && firstString)
        {
            title.append(dKeyDesc[i]);

            firstString = false;
        }
        else if (dKeyShow[i] == true && !firstString)
        {
            title.append("+" + dKeyDesc[i]);

            firstString = false;
        }
    }

    title.append(")");

    chart->setTitle(title);
    axisX->setTitleText("Session");
    axisY->setTitleText("Percent of Session");

    QJsonArray durationArray;

    QString tempName;
    QStringList dKeySeriesNames;

    double max = 0,
           minutes = 0;

    int session = 0,
        maxSession = 0,
        totalSeconds,
        count,
        dIndex = -1;

    for (int i(0); i<dKeyConds.count(); i++)
    {
        lineSeries.append(new QLineSeries);

        lineSeries[i]->setUseOpenGL(true);
        lineSeries[i]->setName(dKeyConds[i]);
        lineSeries[i]->clear();
        lineSeries[i]->show();
        lineSeries[i]->setPointsVisible(true);

        chart->addSeries(lineSeries[i]);

        chart->setAxisX(axisX, lineSeries[i]);
        chart->setAxisY(axisY, lineSeries[i]);

        pointSeries.append(new QScatterSeries);

        pointSeries[i]->setUseOpenGL(true);
        pointSeries[i]->setName("");
        pointSeries[i]->setColor(lineSeries[i]->color());
        pointSeries[i]->clear();
        pointSeries[i]->show();
        pointSeries[i]->setPointsVisible(true);

        chart->addSeries(pointSeries[i]);

        chart->setAxisX(axisX, pointSeries[i]);
        chart->setAxisY(axisY, pointSeries[i]);
    }

    for (int i(0); i<PrimaryReliabilityObjects.count(); i++)
    {
        count = 0;

        if (FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(i).PrimaryFilePath, &json))
        {
            session = json["Session"].toInt();
            totalSeconds = json["SessionDuration"].toInt();

            //durationArray = json["DurationOverall"].toArray();
            durationArray = json[createKeyString("Duration", ui->comboBoxSchedule->currentIndex())].toArray();
            foreach (const QJsonValue collector, durationArray) {
                QJsonObject mObj = collector.toObject();

                dIndex = dKeyDesc.indexOf(mObj["Key"].toString());

                if (dIndex != -1 && dKeyShow[dIndex])
                {
                    count = count + mObj["Count"].toInt();
                }
            }

            lineSeries[dKeyConds.indexOf(PrimaryReliabilityObjects.at(i).Condition)]->append(session, (double) count / (double) totalSeconds);
            pointSeries[dKeyConds.indexOf(PrimaryReliabilityObjects.at(i).Condition)]->append(session, (double) count / (double) totalSeconds);

            if (minutes > 0 && (double) count / minutes > max)
            {
                max = (double) count / (double) totalSeconds;
            }

            if (session > maxSession)
            {
                maxSession = session;
            }
        }
    }

    for (int i(0); i<pointSeries.count(); i++)
    {
        chart->legend()->markers(pointSeries[i])[0]->setVisible(false);
    }

    axisX->setMin(0);
    axisX->setMax(maxSession);

    axisY->setMin(0);
    axisY->setMax(max * 1.1);

}

/**
 * @brief EvaluationViewerDialog::DrawFrequencyPlot
 */
void EvaluationViewerDialog::DrawFrequencyPlot()
{
    chart->removeAllSeries();
    chart->removeAxis(axisX);
    chart->removeAxis(axisY);

    lineSeries.clear();
    pointSeries.clear();

    QString title = QString("Frequency Display, Schedule: " + createKeyString("", ui->comboBoxSchedule->currentIndex()) + " (");
    bool firstString = true;

    for (int i(0); i<fKeyDesc.count(); i++)
    {
        if (fKeyShow[i] == true && firstString)
        {
            title.append(fKeyDesc[i]);

            firstString = false;
        }
        else if (fKeyShow[i] == true && !firstString)
        {
            title.append("+" + fKeyDesc[i]);

            firstString = false;
        }
    }

    title.append(")");

    chart->setTitle(title);
    axisX->setTitleText("Session");
    axisY->setTitleText("Rate/Minute");

    QJsonArray frequencyArray;

    QString tempName;
    QStringList fKeySeriesNames;

    double max = 0,
           minutes = 0;

    int session = 0,
        maxSession = 0,
        totalSeconds,
        count,
        fIndex = -1;

    for (int i(0); i<fKeyConds.count(); i++)
    {
        lineSeries.append(new QLineSeries);

        lineSeries[i]->setUseOpenGL(true);
        lineSeries[i]->setName(fKeyConds[i]);
        lineSeries[i]->clear();
        lineSeries[i]->show();
        lineSeries[i]->setPointsVisible(true);

        chart->addSeries(lineSeries[i]);

        chart->setAxisX(axisX, lineSeries[i]);
        chart->setAxisY(axisY, lineSeries[i]);

        pointSeries.append(new QScatterSeries);

        pointSeries[i]->setUseOpenGL(true);
        pointSeries[i]->setName("");
        pointSeries[i]->setColor(lineSeries[i]->color());
        pointSeries[i]->clear();
        pointSeries[i]->show();
        pointSeries[i]->setPointsVisible(true);

        chart->addSeries(pointSeries[i]);

        chart->setAxisX(axisX, pointSeries[i]);
        chart->setAxisY(axisY, pointSeries[i]);
    }

    for (int i(0); i<PrimaryReliabilityObjects.count(); i++)
    {
        count = 0;

        if (FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(i).PrimaryFilePath, &json))
        {
            session = json["Session"].toInt();
            totalSeconds = json["SessionDuration"].toInt();

            frequencyArray = json[createKeyString("Frequency", ui->comboBoxSchedule->currentIndex())].toArray();
            foreach (const QJsonValue collector, frequencyArray) {
                QJsonObject mObj = collector.toObject();

                fIndex = fKeyDesc.indexOf(mObj["Key"].toString());

                if (fIndex != -1 && fKeyShow[fIndex])
                {
                    count = count + mObj["Count"].toInt();
                }
            }

            minutes = ((double) (totalSeconds / 1000)) / (double) 60;

            lineSeries[fKeyConds.indexOf(PrimaryReliabilityObjects.at(i).Condition)]->append(session, (double) count / minutes);
            pointSeries[fKeyConds.indexOf(PrimaryReliabilityObjects.at(i).Condition)]->append(session, (double) count / minutes);

            if ((double) count / minutes > max)
            {
                max = (double) count / minutes;
            }

            if (session > maxSession)
            {
                maxSession = session;
            }
        }
    }

    for (int i(0); i<pointSeries.count(); i++)
    {
        chart->legend()->markers(pointSeries[i])[0]->setVisible(false);
    }

    axisX->setMin(0);
    axisX->setMax(maxSession);

    axisY->setMin(0);
    axisY->setMax(max * 1.1);
}
