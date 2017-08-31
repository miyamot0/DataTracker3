#include "evaluationviewerdialog.h"
#include "ui_evaluationviewerdialog.h"

#include "filetools.h"

EvaluationViewerDialog::EvaluationViewerDialog(QString mCurrentWorkingDirectory = "", QWidget *parent) :
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
}

EvaluationViewerDialog::~EvaluationViewerDialog()
{
    delete ui;
}

void EvaluationViewerDialog::WorkUpdate(QString update)
{
    qDebug() << "WORK_UPDATE: " << update;
}

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
}

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
}

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

    qSort(PrimaryReliabilityObjects.begin(), PrimaryReliabilityObjects.end(),
          [](const ReliabilityParse a, const ReliabilityParse b) -> bool {return a.SessionNumber < b.SessionNumber;});


    for (int i(0); i<PrimaryReliabilityObjects.count(); i++)
    {
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());

        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 0, new QTableWidgetItem(QString::number(PrimaryReliabilityObjects.at(i).SessionNumber)));
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 1, new QTableWidgetItem(PrimaryReliabilityObjects.at(i).Condition));
        ui->tableWidget->setItem(ui->tableWidget->rowCount() - 1, 2, new QTableWidgetItem(PrimaryReliabilityObjects.at(i).Collector));
    }

    DrawPlot();
}

void EvaluationViewerDialog::DrawPlot()
{
    QDateTime startTime, endTime;
    int totalSecs;

    QList<KeySetEntry> FrequencyKeys;
    QJsonArray frequencyArray;

    QList<KeySetEntry> DurationKeys;
    QJsonArray durationArray;

    QList<SessionEvent> PressedKeys;
    QJsonArray pressedKeysJson;

    QList<QString> fKeyDesc, dKeyDesc;

    int max, fIndex, secs, session;
    double minutes;

    for (int i(0); i<PrimaryReliabilityObjects.count(); i++)
    {
        if (FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(i).PrimaryFilePath, &json))
        {
            chart->removeAllSeries();
            lineSeries.clear();

            fKeyDesc.clear();
            //fKeySum.clear();

            dKeyDesc.clear();
            //fKeySum.clear();

            startTime = QDateTime(QDateTime::fromString(json["StartTime"].toString()));
            endTime = QDateTime(QDateTime::fromString(json["EndTime"].toString()));

            session = json["Session"].toInt();

            totalSecs = (int)((double) json["SessionDuration"].toInt() / 1000);

            minutes = ((double) totalSecs) / 60;

            FrequencyKeys.clear();
            frequencyArray = json["FrequencyKeys"].toArray();
            foreach (const QJsonValue collector, frequencyArray) {
                QJsonObject mObj = collector.toObject();

                KeySetEntry mEntry;
                mEntry.KeyCode = mObj["Code"].toInt();
                mEntry.KeyDescription = mObj["Description"].toString();
                mEntry.KeyName = mObj["Name"].toString();

                FrequencyKeys.append(mEntry);

                fKeyDesc.append(mObj["Description"].toString());
                //fKeySum.append(0);

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

            int count;

            frequencyArray = json["FrequencyOverall"].toArray();
            foreach (const QJsonValue collector, frequencyArray) {
                QJsonObject mObj = collector.toObject();

                count = mObj["Count"].toInt();

                fIndex = fKeyDesc.indexOf(mObj["Key"].toString());

                if (fIndex != -1)
                {
                    *lineSeries[fIndex] << QPointF((double) session, ((double) count / minutes));

                    qDebug() << " Session (" << session << " : " << fIndex <<
                                ") bx: " << fKeyDesc[fIndex] <<
                                " rate: "  << (double)count/minutes <<
                                " min: " << minutes <<
                                " count: " << count;

                    //if (fKeySum[fIndex] > max)
                    //{
                    //    max = fKeySum[fIndex];
                    //}

                    if ((int) (count / minutes) > max)
                    {
                        //max = (int)(count / minutes);
                        max = 1;
                    }
                }

            }

            /*

            DurationKeys.clear();
            durationArray = json["DurationKeys"].toArray();
            foreach (const QJsonValue collector, durationArray) {
                QJsonObject mObj = collector.toObject();

                KeySetEntry mEntry;
                mEntry.KeyCode = mObj["Code"].toInt();
                mEntry.KeyDescription = mObj["Description"].toString();
                mEntry.KeyName = mObj["Name"].toString();

                DurationKeys.append(mEntry);

                dKeyDesc.append(mObj["Description"].toString());
                //dKeySum.append(0.0);

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
                */

            /*
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
                    //secs = startTime.secsTo(event.TimePressed);
                    //fKeySum[fIndex] = fKeySum[fIndex] + 1;
                    //*lineSeries[fIndex] << QPointF(secs, fKeySum[fIndex]);
                    //if (fKeySum[fIndex] > max)
                    //{
                    //    max = fKeySum[fIndex];
                    //}
                }
            }
            */



            // duration

            /*
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

                            *lineSeries2[i] << QPointF(startSecs, runningSum);

                            runningSum = runningSum + (startSecs + endSecs);

                            *lineSeries2[i] << QPointF(endSecs, runningSum);

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
                    endSecs = ((double) startTime.msecsTo(endTime)) / 1000;

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

            */
        }
    }

    axisY->setMax(2);
    axisX->setMax(session + 1);

    /*
    temp = PrimaryReliabilityObjects.at(currentRow);

    result = FileTools::ReadSessionFromJSON(temp.PrimaryFilePath, &json);

    if (result)
    {


    }
    */
}

void EvaluationViewerDialog::on_pushButton_clicked()
{

}
