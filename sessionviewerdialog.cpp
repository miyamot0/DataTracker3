
#include <QDebug>
#include <QThread>
#include <QDir>
#include <QDirIterator>
#include <QJsonObject>

#include "sessionviewerdialog.h"
#include "ui_sessionviewerdialog.h"

#include "filetools.h"
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

    // Duration Plots

    lineSeries2.clear();

    chart2 = new QChart();
    chart2->layout()->setContentsMargins(0, 0, 0, 0);
    chart2->setBackgroundRoundness(0);
    chart2->setTitle("");
    chart2->setTitleFont(QFont("Serif", 10, -1, false));
    chart2->setTitleBrush(Qt::black);

    axisX2 = new QValueAxis;
    axisX2->applyNiceNumbers();
    axisX2->setGridLineColor(Qt::transparent);
    axisX2->setTitleText(tr("Seconds"));
    axisX2->setTitleFont(QFont("Serif", 10, -1, false));
    axisX2->setTitleBrush(Qt::black);
    axisX2->setMin(0);
    axisX2->setLabelsFont(QFont("Serif", 10, -1, false));
    axisX2->setLabelFormat(QString("%.0f"));
    axisX2->setLabelsBrush(Qt::black);
    axisX2->setLabelsColor(Qt::black);
    axisX2->setLinePenColor(Qt::black);
    axisX2->setLinePen(QPen(Qt::black));

    axisY2 = new QValueAxis;
    axisY2->applyNiceNumbers();
    axisY2->setGridLineColor(Qt::transparent);
    axisY2->setTitleText(tr("Total Time (s)"));
    axisY2->setTitleFont(QFont("Serif", 10, -1, false));
    axisY2->setTitleBrush(Qt::black);
    axisY2->setLabelsFont(QFont("Serif", 10, -1, false));
    axisY2->setLabelsBrush(Qt::black);
    axisY2->setLabelsColor(Qt::black);
    axisY2->setMin(0);
    axisY2->setMax(100);
    axisY2->setLinePenColor(Qt::black);
    axisY2->setLinePen(QPen(Qt::black));

    chartView2 = new QChartView(chart2);
    chartView2->setRenderHint(QPainter::Antialiasing);

    ui->plotLayout2->addWidget(chartView2);

    axisY2->setMin(0);
}

SessionViewerDialog::~SessionViewerDialog()
{
    delete ui;
}

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
}

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
}

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
}

void SessionViewerDialog::WorkUpdate(QString update)
{
    qDebug() << "WORK_UPDATE: " << update;
}

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

void SessionViewerDialog::on_tableWidget_currentCellChanged(int currentRow, int, int, int)
{
    if (currentRow < 0)
    {
        return;
    }

    ReliabilityParse temp = PrimaryReliabilityObjects.at(currentRow);

    bool result = FileTools::ReadSessionFromJSON(temp.PrimaryFilePath, &json);

    if (result)
    {
        chart->removeAllSeries();
        chart2->removeAllSeries();

        lineSeries.clear();
        lineSeries2.clear();

        fKeySet.clear();
        fKeySum.clear();

        dKeySet.clear();
        fKeySum.clear();

        QDateTime startTime = QDateTime(QDateTime::fromString(json["StartTime"].toString()));
        QDateTime endTime = QDateTime(QDateTime::fromString(json["EndTime"].toString()));

        int totalSecs = (int)((double) json["SessionDuration"].toInt() / 1000);

        QList<KeySetEntry> FrequencyKeys;
        QJsonArray frequencyArray = json["FrequencyKeys"].toArray();
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

        QList<KeySetEntry> DurationKeys;
        QJsonArray durationArray = json["DurationKeys"].toArray();
        foreach (const QJsonValue collector, durationArray) {
            QJsonObject mObj = collector.toObject();

            KeySetEntry mEntry;
            mEntry.KeyCode = mObj["Code"].toInt();
            mEntry.KeyDescription = mObj["Description"].toString();
            mEntry.KeyName = mObj["Name"].toString();

            DurationKeys.append(mEntry);

            dKeySet.append(mObj["Code"].toInt());
            dKeySum.append(0.0);

            lineSeries2.append(new QLineSeries);

            lineSeries2[lineSeries2.count() - 1]->setUseOpenGL(true);
            lineSeries2[lineSeries2.count() - 1]->setName(mObj["Description"].toString());
            lineSeries2[lineSeries2.count() - 1]->clear();
            lineSeries2[lineSeries2.count() - 1]->show();

            chart2->addSeries(lineSeries2[lineSeries2.count() - 1]);

            chart2->setAxisX(axisX2, lineSeries2[lineSeries2.count() - 1]);
            chart2->setAxisY(axisY2, lineSeries2[lineSeries2.count() - 1]);

            *lineSeries2[lineSeries2.count() - 1] << QPointF(0, 0);
        }

        QList<SessionEvent> PressedKeys;
        QJsonArray pressedKeysJson = json["PressedKeys"].toArray();
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

        int max = 0;

        foreach (const SessionEvent event, PressedKeys)
        {
            int fIndex = fKeySet.indexOf(event.KeyEntered.KeyCode);

            if (fIndex != -1)
            {
                int secs = startTime.secsTo(event.TimePressed);

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

        QDateTime prev, after;
        bool waitingForNext;
        double runningSum;
        int temp;

        max = 0;

        for (int i(0); i<dKeySet.count(); i++)
        {
            temp = dKeySet.at(i);
            waitingForNext = false;
            runningSum = 0;
            //runs.clear();

            foreach(SessionEvent event, PressedKeys)
            {
                if (event.KeyEntered.KeyCode == temp)
                {
                    if (waitingForNext)
                    {
                        after = event.TimePressed;

                        double startSecs = ((double) startTime.msecsTo(prev)) / 1000;
                        double endSecs = ((double) startTime.msecsTo(after)) / 1000;

                        *lineSeries2[i] << QPointF(startSecs, runningSum);

                        runningSum = runningSum + (startSecs + endSecs);

                        *lineSeries2[i] << QPointF(endSecs, runningSum);

                        waitingForNext = false;

                        qDebug() << "Key code: " << temp << " run completed";
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
                double startSecs = ((double) startTime.msecsTo(prev)) / 1000;
                double endSecs = ((double) startTime.msecsTo(endTime)) / 1000;

                *lineSeries2[i] << QPointF(startSecs, runningSum);

                runningSum = runningSum + (startSecs + endSecs);

                *lineSeries2[i] << QPointF(endSecs, runningSum);
            }

            if ((int) runningSum > max)
            {
                max = (int) runningSum;
            }
        }

        axisY2->setMax(max + 1);
        axisX2->setMax(totalSecs);
    }
}
