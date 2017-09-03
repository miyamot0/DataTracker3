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

#include <QThread>
#include <QDir>
#include <QDirIterator>
#include <QJsonObject>

#include "sessionviewerdialog.h"
#include "ui_sessionviewerdialog.h"

#include "filetools.h"
#include "windowtools.h"
#include "reliabilityparse.h"

SessionViewerDialog::SessionViewerDialog(QString mCurrentWorkingDirectory, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SessionViewerDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("Session Viewer"));

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
    axisY->setMax(100);
    axisY->setLinePenColor(Qt::black);
    axisY->setLinePen(QPen(Qt::black));

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    ui->plotLayout->addWidget(chartView);

    axisY->setMin(0);

    WindowTools::SetDialogFixedMaximize(this);
}

SessionViewerDialog::~SessionViewerDialog()
{
    delete ui;
}

/**
 * @brief SessionViewerDialog::WorkUpdate
 * @param update
 */
void SessionViewerDialog::WorkUpdate(QString update)
{
    qDebug() << "WORK_UPDATE: " << update;
}

/**
 * @brief SessionViewerDialog::WorkFinished
 * @param finalResult
 * @param action
 */
void SessionViewerDialog::WorkFinished(DirectoryParse finalResult, ParseTypes::ParseAction action)
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
 * @brief SessionViewerDialog::on_comboBoxGroup_currentIndexChanged
 * @param index
 */
void SessionViewerDialog::on_comboBoxGroup_currentIndexChanged(int index)
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
        DrawBlankPlot();

        ui->comboBoxIndividual->setEnabled(true);
        ui->comboBoxEvaluation->setEnabled(false);
        ui->comboBoxDimension->setEnabled(false);
    }
}

/**
 * @brief SessionViewerDialog::on_comboBoxIndividual_currentIndexChanged
 * @param index
 */
void SessionViewerDialog::on_comboBoxIndividual_currentIndexChanged(int index)
{
    if (index == 0)
    {
        while (ui->comboBoxEvaluation->count() > 1)
        {
            ui->comboBoxEvaluation->removeItem(1);
        }

        ui->tableWidget->setRowCount(0);

        ui->comboBoxEvaluation->setEnabled(false);
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

        DrawBlankPlot();

        ui->comboBoxEvaluation->setEnabled(true);
        ui->comboBoxDimension->setEnabled(false);
    }
}

/**
 * @brief SessionViewerDialog::on_comboBoxEvaluation_currentIndexChanged
 * @param index
 */
void SessionViewerDialog::on_comboBoxEvaluation_currentIndexChanged(int index)
{
    if (index == 0)
    {
        ui->tableWidget->setRowCount(0);

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
            ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 3, new QTableWidgetItem(formatReli(PrimaryReliabilityObjects.at(i).Reli)));
        }

        ui->comboBoxDimension->setCurrentIndex(0);

        DrawBlankPlot();

        ui->comboBoxDimension->setEnabled(true);
    }
}

/**
 * @brief SessionViewerDialog::on_comboBoxSchedule_currentIndexChanged
 * @param index
 */
void SessionViewerDialog::on_comboBoxSchedule_currentIndexChanged(int)
{
    ui->comboBoxDimension->setCurrentIndex(0);
}

/**
 * @brief SessionViewerDialog::on_comboBoxDimension_currentIndexChanged
 * @param index
 */
void SessionViewerDialog::on_comboBoxDimension_currentIndexChanged(int index)
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

        mSeriesSelect.setWindowTitle(tr("Select Frequency Keys"));
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

        QStringList mDurationKeys;

        for (QString mKey : dKeyDesc)
        {
            mDurationKeys.append(mKey.split("-")[0]);
        }

        dKeyDesc = mDurationKeys.toSet().toList();

        mSeriesSelect.setWindowTitle(tr("Select Duration Keys"));
        mSeriesSelect.AddOptions(dKeyDesc);

        if (mSeriesSelect.exec() != QDialog::Rejected)
        {
            dKeyShow = mSeriesSelect.GetBoolList();

            DrawDurationPlot();
        }

    }
    else
    {
        DrawBlankPlot();
    }
}

/**
 * @brief SessionViewerDialog::GetDurationKeySets
 */
void SessionViewerDialog::GetDurationKeySets()
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
 * @brief SessionViewerDialog::GetFrequencyKeySets
 */
void SessionViewerDialog::GetFrequencyKeySets()
{
    QJsonArray frequencyArray;

    if (PrimaryReliabilityObjects.count() > 0 && FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(0).PrimaryFilePath, &json))
    {
        fKeyDesc.clear();

        // Get all keys
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
 * @brief SessionViewerDialog::on_pushButton_clicked
 */
void SessionViewerDialog::on_pushButton_clicked()
{
    on_comboBoxDimension_currentIndexChanged(ui->comboBoxDimension->currentIndex());
}

/**
 * @brief SessionViewerDialog::DrawBlankPlot
 */
void SessionViewerDialog::DrawBlankPlot()
{
    chart->setTitle("");
    chart->removeAllSeries();
    chart->removeAxis(axisX);
    chart->removeAxis(axisY);

    axisX->setTitleText("Counts");
    axisY->setTitleText("Session");
}

/**
 * @brief SessionViewerDialog::DrawFrequencyPlot
 */
void SessionViewerDialog::DrawFrequencyPlot()
{
    chart->removeAllSeries();
    chart->removeAxis(axisX);
    chart->removeAxis(axisY);

    axisX->setTitleText("Seconds");
    axisY->setTitleText("Behavior Counts");

    QJsonArray frequencyArray;
    QString tempName;

    fKeySeriesNames.clear();

    if (PrimaryReliabilityObjects.count() > 0 && FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(0).PrimaryFilePath, &json))
    {
        chart->removeAllSeries();
        lineSeries.clear();

        if (FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(0).PrimaryFilePath, &json))
        {
            frequencyArray = json["FrequencyKeys"].toArray();
            foreach (const QJsonValue collector, frequencyArray) {
                QJsonObject mObj = collector.toObject();

                int fIndex = fKeyDesc.indexOf(mObj["Description"].toString());

                if (fIndex != -1 && fKeyShow.at(fIndex))
                {
                    tempName = mObj["Description"].toString();

                    if (!fKeySeriesNames.contains(tempName))
                    {
                        fKeySeriesNames.append(tempName);

                        lineSeries.append(new QLineSeries);

                        lineSeries[fKeySeriesNames.indexOf(tempName)]->setUseOpenGL(true);
                        lineSeries[fKeySeriesNames.indexOf(tempName)]->setName(tempName);
                        lineSeries[fKeySeriesNames.indexOf(tempName)]->clear();
                        lineSeries[fKeySeriesNames.indexOf(tempName)]->show();

                        *lineSeries[fKeySeriesNames.indexOf(tempName)] << QPointF(0, 0);

                        chart->addSeries(lineSeries[fKeySeriesNames.indexOf(tempName)]);

                        chart->setAxisX(axisX, lineSeries[fKeySeriesNames.indexOf(tempName)]);
                        chart->setAxisY(axisY, lineSeries[fKeySeriesNames.indexOf(tempName)]);

                    }
                }
            }
        }

        DrawFrequencySeries(ui->tableWidget->currentRow());
    }
}

/**
 * @brief SessionViewerDialog::DrawDurationPlot
 */
void SessionViewerDialog::DrawDurationPlot()
{
    chart->removeAllSeries();
    chart->removeAxis(axisX);
    chart->removeAxis(axisY);

    axisX->setTitleText("Seconds");
    axisY->setTitleText("Total Seconds");

    QJsonArray durationArray;
    QString tempName;

    dKeySeriesNames.clear();   

    if (PrimaryReliabilityObjects.count() > 0 && FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(0).PrimaryFilePath, &json))
    {
        chart->removeAllSeries();
        lineSeries.clear();

        if (FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(0).PrimaryFilePath, &json))
        {
            durationArray = json["DurationKeys"].toArray();
            foreach (const QJsonValue collector, durationArray) {
                QJsonObject mObj = collector.toObject();

                int dIndex = dKeyDesc.indexOf(mObj["Description"].toString());

                if (dIndex != -1 && dKeyShow.at(dIndex))
                {
                    tempName = mObj["Description"].toString();

                    if (!dKeySeriesNames.contains(tempName))
                    {
                        dKeySeriesNames.append(tempName);

                        lineSeries.append(new QLineSeries);

                        lineSeries[dKeySeriesNames.indexOf(tempName)]->setUseOpenGL(true);
                        lineSeries[dKeySeriesNames.indexOf(tempName)]->setName(tempName);
                        lineSeries[dKeySeriesNames.indexOf(tempName)]->clear();
                        lineSeries[dKeySeriesNames.indexOf(tempName)]->show();

                        *lineSeries[dKeySeriesNames.indexOf(tempName)] << QPointF(0, 0);

                        chart->addSeries(lineSeries[dKeySeriesNames.indexOf(tempName)]);

                        chart->setAxisX(axisX, lineSeries[dKeySeriesNames.indexOf(tempName)]);
                        chart->setAxisY(axisY, lineSeries[dKeySeriesNames.indexOf(tempName)]);
                    }
                }
            }
        }

        DrawDurationSeries(ui->tableWidget->currentRow());
    }
}

/**
 * @brief SessionViewerDialog::DrawFrequencySeries
 * @param index
 */
void SessionViewerDialog::DrawFrequencySeries(int index)
{
    if (index < 0)
    {
        if (ui->tableWidget->rowCount() < 1)
        {
            return;
        }
        else
        {
            index = 0;

            ui->tableWidget->selectRow(index);
        }
    }

    temp = PrimaryReliabilityObjects.at(index);
    result = FileTools::ReadSessionFromJSON(temp.PrimaryFilePath, &json);

    int secs;
    max = 1;

    QString tempName;

    if (result)
    {
        fKeySum.clear();

        foreach (QLineSeries * series, lineSeries)
        {
            series->clear();
        }

        foreach (QString key, fKeyDesc) {
            fKeySum.append(0);
        }

        startTime = QDateTime(QDateTime::fromString(json["StartTime"].toString()));
        totalSecs = (int)((double) json["SessionDuration"].toInt() / 1000);

        chart->setTitle(QString("Session #: %1").arg(json["Session"].toInt()));

        pressedKeysJson = json["PressedKeys"].toArray();

        foreach (const QJsonValue collector, pressedKeysJson) {
            QJsonObject mObj = collector.toObject();

            int fIndex = fKeyDesc.indexOf(mObj["KeyDescription"].toString());

            if (fIndex != -1 && fKeyShow[fIndex] == true)
            {
                tempName = (mObj["KeyDescription"].toString());
                secs = startTime.secsTo(QDateTime::fromString(mObj["TimePressed"].toString()));

                *lineSeries[fKeySeriesNames.indexOf(tempName)] << QPointF(secs, fKeySum[fKeySeriesNames.indexOf(tempName)]);
                fKeySum[fKeySeriesNames.indexOf(tempName)] = fKeySum[fKeySeriesNames.indexOf(tempName)] + 1;
                *lineSeries[fKeySeriesNames.indexOf(tempName)] << QPointF(secs, fKeySum[fKeySeriesNames.indexOf(tempName)]);

                if (fKeySum[fKeySeriesNames.indexOf(tempName)] > (int) max)
                {
                    max = (double) fKeySum[fKeySeriesNames.indexOf(tempName)];
                }
            }
        }

        // Cap off
        for (int i(0); i<lineSeries.count(); i++)
        {
            lineSeries.at(i)->append(totalSecs, lineSeries.at(i)->at(lineSeries.at(i)->count() - 1).y());
        }

        axisY->setMax(max + 1);
        axisX->setMax(totalSecs);

        max = 0;
    }
}

/**
 * @brief SessionViewerDialog::DrawDurationSeries
 * @param index
 */
void SessionViewerDialog::DrawDurationSeries(int index)
{
    if (index < 0)
    {
        if (ui->tableWidget->rowCount() < 1)
        {
            return;
        }
        else
        {
            index = 0;

            ui->tableWidget->selectRow(index);
        }
    }

    temp = PrimaryReliabilityObjects.at(index);
    result = FileTools::ReadSessionFromJSON(temp.PrimaryFilePath, &json);

    max = 1;

    if (result)
    {
        dKeySum.clear();

        foreach (QLineSeries * series, lineSeries)
        {
            series->clear();
        }

        foreach (QString key, dKeyDesc) {
            dKeySum.append(0);
        }

        startTime = QDateTime(QDateTime::fromString(json["StartTime"].toString()));
        totalSecs = (int)((double) json["SessionDuration"].toInt() / 1000);

        chart->setTitle(QString("Session #: %1").arg(json["Session"].toInt()));

        pressedKeysJson = json["PressedKeys"].toArray();

        QString tempKeyCode;

        for (int i(0); i<dKeySeriesNames.count(); i++)
        {
            tempKeyCode = dKeySeriesNames.at(i);
            waitingForNext = false;
            runningSum = 0;

            foreach (const QJsonValue collector, pressedKeysJson) {
                QJsonObject mObj = collector.toObject();

                if (mObj["KeyDescription"].toString() == tempKeyCode)
                {
                    if (waitingForNext)
                    {
                        after = QDateTime(QDateTime::fromString(mObj["TimePressed"].toString()));

                        startSecs = ((double) startTime.msecsTo(prev)) / 1000;
                        endSecs = ((double) startTime.msecsTo(after)) / 1000;

                        *lineSeries[dKeySeriesNames.indexOf(tempKeyCode)] << QPointF(startSecs, runningSum);
                        runningSum = runningSum + (endSecs - startSecs);
                        *lineSeries[dKeySeriesNames.indexOf(tempKeyCode)] << QPointF(endSecs, runningSum);

                        if (runningSum > (int) max)
                        {
                            max = (double) runningSum;
                        }

                        waitingForNext = false;
                    }
                    else
                    {
                        prev = QDateTime(QDateTime::fromString(mObj["TimePressed"].toString()));
                        waitingForNext = true;
                    }

                }
            }

            if (waitingForNext)
            {
                startSecs = ((double) startTime.msecsTo(prev)) / 1000;

                *lineSeries[dKeySeriesNames.indexOf(tempKeyCode)] << QPointF(startSecs, runningSum);
                runningSum = runningSum + (totalSecs - startSecs);
                *lineSeries[dKeySeriesNames.indexOf(tempKeyCode)] << QPointF(totalSecs, runningSum);
            }

            if ((int) runningSum > max)
            {
                max = (int) runningSum;
            }
        }

        // Cap off
        for (int i(0); i<lineSeries.count(); i++)
        {
            lineSeries.at(i)->append(totalSecs, lineSeries.at(i)->at(lineSeries.at(i)->count() - 1).y());
        }

        axisY->setMax(max + 1);
        axisX->setMax(totalSecs + 1);

        max = 0;
    }
}

/**
 * @brief SessionViewerDialog::on_tableWidget_currentCellChanged
 * @param currentRow
 */
void SessionViewerDialog::on_tableWidget_currentCellChanged(int currentRow, int, int, int)
{
    if (currentRow < 0)
    {
        return;
    }
    else if (ui->comboBoxDimension->currentIndex() == 1)
    {
        DrawFrequencySeries(currentRow);
    }
    else if (ui->comboBoxDimension->currentIndex() == 2)
    {
        DrawDurationSeries(currentRow);
    }
}


