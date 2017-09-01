
#include <QDebug>
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
    DrawBlankPlot();
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
    DrawBlankPlot();
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

                            PrimaryReliabilityObjects.append(mReliObj);
                        }
                    }
                }
            }
        }
    }

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

        mSeriesSelect.AddOptions(fKeyDesc);
        mSeriesSelect.exec();

        fKeyShow = mSeriesSelect.GetBoolList();

        DrawFrequencyPlot();
    }
    else if (index == 2)
    {
        /*
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
        */
    }
    else
    {
        DrawBlankPlot();
    }
}

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

            SessionEvent mEntry;
            mEntry.KeyEntered.KeyCode = mObj["KeyCode"].toInt();
            mEntry.KeyEntered.KeyDescription = mObj["KeyDescription"].toString();
            mEntry.KeyEntered.KeyName = mObj["KeyName"].toString();
            mEntry.TimePressed = QDateTime(QDateTime::fromString(mObj["TimePressed"].toString()));

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

        axisY->setMax(max + 1);
        axisX->setMax(totalSecs);

        max = 0;
    }
}

void SessionViewerDialog::on_tableWidget_currentCellChanged(int currentRow, int, int, int)
{
    if (currentRow < 0)
    {
        return;
    }

    if (ui->comboBoxDimension->currentIndex() == 1)
    {
        DrawFrequencySeries(currentRow);
    }

    return;

    temp = PrimaryReliabilityObjects.at(currentRow);

    result = FileTools::ReadSessionFromJSON(temp.PrimaryFilePath, &json);

    if (result)
    {
        chart->removeAllSeries();
        chart->removeAxis(axisX);

        lineSeries.clear();

        fKeySet.clear();
        fKeySum.clear();

        dKeySet.clear();
        fKeySum.clear();

        startTime = QDateTime(QDateTime::fromString(json["StartTime"].toString()));
        endTime = QDateTime(QDateTime::fromString(json["EndTime"].toString()));
        totalSecs = (int)((double) json["SessionDuration"].toInt() / 1000);

        FrequencyKeys.clear();
        frequencyArray = json["FrequencyKeys"].toArray();
        foreach (const QJsonValue collector, frequencyArray) {
            QJsonObject mObj = collector.toObject();

            KeySetEntry mEntry;
            mEntry.KeyCode = mObj["Code"].toInt();
            mEntry.KeyDescription = mObj["Description"].toString();
            mEntry.KeyName = mObj["Name"].toString();

            FrequencyKeys.append(mEntry);

            fKeySet.append(mObj["Code"].toInt());
            fKeySum.append(0);

            lineSeries.append(new QLineSeries);

            lineSeries[lineSeries.count() - 1]->setUseOpenGL(true);
            lineSeries[lineSeries.count() - 1]->setName(mObj["Description"].toString());
            lineSeries[lineSeries.count() - 1]->clear();
            lineSeries[lineSeries.count() - 1]->show();

            chart->addSeries(lineSeries[lineSeries.count() - 1]);

            chart->setAxisX(axisX, lineSeries[lineSeries.count() - 1]);
            chart->setAxisY(axisY, lineSeries[lineSeries.count() - 1]);

            *lineSeries[lineSeries.count() - 1] << QPointF(0, 0);
        }

        DurationKeys.clear();
        durationArray = json["DurationKeys"].toArray();
        foreach (const QJsonValue collector, durationArray) {
            QJsonObject mObj = collector.toObject();

            KeySetEntry mEntry;
            mEntry.KeyCode = mObj["Code"].toInt();
            mEntry.KeyDescription = mObj["Description"].toString();
            mEntry.KeyName = mObj["Name"].toString();

            DurationKeys.append(mEntry);

            dKeySet.append(mObj["Code"].toInt());
            dKeySum.append(0.0);

            //lineSeries2.append(new QLineSeries);

            //lineSeries2[lineSeries2.count() - 1]->setUseOpenGL(true);
            //lineSeries2[lineSeries2.count() - 1]->setName(mObj["Description"].toString());
            //lineSeries2[lineSeries2.count() - 1]->clear();
            //lineSeries2[lineSeries2.count() - 1]->show();

            //chart2->addSeries(lineSeries2[lineSeries2.count() - 1]);

            //chart2->setAxisX(axisX2, lineSeries2[lineSeries2.count() - 1]);
            //chart2->setAxisY(axisY2, lineSeries2[lineSeries2.count() - 1]);

            //*lineSeries2[lineSeries2.count() - 1] << QPointF(0, 0);
        }

        PressedKeys.clear();
        pressedKeysJson = json["PressedKeys"].toArray();
        foreach (const QJsonValue collector, pressedKeysJson) {
            QJsonObject mObj = collector.toObject();

            SessionEvent mEntry;
            mEntry.KeyEntered.KeyCode = mObj["KeyCode"].toInt();
            mEntry.KeyEntered.KeyDescription = mObj["KeyDescription"].toString();
            mEntry.KeyEntered.KeyName = mObj["KeyName"].toString();
            //mEntry.MeasurementType = mObj["Measurement"].toString();
            //mEntry.ScheduleType = mObj["Schedule"].toString();
            mEntry.TimePressed = QDateTime(QDateTime::fromString(mObj["TimePressed"].toString()));

            PressedKeys.append(mEntry);
        }

        max = 0;

        foreach (const SessionEvent event, PressedKeys)
        {
            fIndex = fKeySet.indexOf(event.KeyEntered.KeyCode);

            if (fIndex != -1)
            {
                secs = startTime.secsTo(event.TimePressed);

                fKeySum[fIndex] = fKeySum[fIndex] + 1;

                *lineSeries[fIndex] << QPointF(secs, fKeySum[fIndex]);

                if (fKeySum[fIndex] > max)
                {
                    max = fKeySum[fIndex];
                }
            }
        }

        axisY->setMax(max + 1);
        axisX->setMax(totalSecs);

        // duration

        max = 0;

        for (int i(0); i<dKeySet.count(); i++)
        {
            tempKeyCode = dKeySet.at(i);
            waitingForNext = false;
            runningSum = 0;

            foreach(SessionEvent event, PressedKeys)
            {
                if (event.KeyEntered.KeyCode == tempKeyCode)
                {
                    if (waitingForNext)
                    {
                        after = event.TimePressed;

                        startSecs = ((double) startTime.msecsTo(prev)) / 1000;
                        endSecs = ((double) startTime.msecsTo(after)) / 1000;

                        //*lineSeries2[i] << QPointF(startSecs, runningSum);

                        runningSum = runningSum + (endSecs - startSecs);

                        //*lineSeries2[i] << QPointF(endSecs, runningSum);

                        waitingForNext = false;
                    }
                    else
                    {
                        prev = event.TimePressed;
                        waitingForNext = true;
                    }
                }
            }

            if (waitingForNext)
            {
                startSecs = ((double) startTime.msecsTo(prev)) / 1000;

                //*lineSeries2[i] << QPointF(startSecs, runningSum);

                runningSum = runningSum + (totalSecs - startSecs);

                //*lineSeries2[i] << QPointF(totalSecs, runningSum);
            }

            if ((int) runningSum > max)
            {
                max = (int) runningSum;
            }
        }

        //axisY2->setMax(max + 1);
        //axisX2->setMax(totalSecs);
    }
}



