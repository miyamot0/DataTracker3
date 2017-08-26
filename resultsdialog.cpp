#include "resultsdialog.h"
#include "ui_resultsdialog.h"

#include "sessionevent.h"
#include "filetools.h"

#include <QStringListModel>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>

ResultsDialog::ResultsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResultsDialog)
{
    ui->setupUi(this);
}

void ResultsDialog::SetParameters(QString group, QString individual, QString evaluation,
                                  QString condition, QString therapist, QString keySet,
                                  QString collector, QString role)
{
    Group = group;
    Individual = individual;
    Evaluation = evaluation;
    Condition = condition;
    Therapist = therapist;
    KeySetName = keySet;
    Collector = collector;
    Role = role;
}

void ResultsDialog::SetKeySet(KeySet currKeySet)
{
    CurrentKeySet = currKeySet;
}

void ResultsDialog::BuildTables()
{
    for (int i(0); i<FrequencyOverall.count(); i++)
    {
        ui->tableMainFreqAll->insertRow(ui->tableMainFreqAll->rowCount());
        ui->tableMainFreqAll->setItem(ui->tableMainFreqAll->rowCount() - 1, 0,
                                      new QTableWidgetItem(FrequencyOverall.at(i).first));
        ui->tableMainFreqAll->setItem(ui->tableMainFreqAll->rowCount() - 1, 1,
                                      new QTableWidgetItem(QString::number(FrequencyOverall.at(i).second)));
        ui->tableMainFreqAll->setItem(ui->tableMainFreqAll->rowCount() - 1, 2,
                                      new QTableWidgetItem(formatRate(FrequencyOverall.at(i).second, TimeOverall)));
    }

    for (int i(0); i<DurationOverall.count(); i++)
    {
        ui->tableMainDurAll->insertRow(ui->tableMainDurAll->rowCount());
        ui->tableMainDurAll->setItem(ui->tableMainDurAll->rowCount() - 1, 0,
                                      new QTableWidgetItem(DurationOverall.at(i).first));
        ui->tableMainDurAll->setItem(ui->tableMainDurAll->rowCount() - 1, 1,
                                      new QTableWidgetItem(formatTimeLabel(DurationOverall.at(i).second)));
        ui->tableMainDurAll->setItem(ui->tableMainDurAll->rowCount() - 1, 2,
                                      new QTableWidgetItem(formatPercentage(DurationOverall.at(i).second, TimeOverall)));
    }

    for (int i(0); i<FrequencyOne.count(); i++)
    {
        ui->tableMainFreqOne->insertRow(ui->tableMainFreqOne->rowCount());
        ui->tableMainFreqOne->setItem(ui->tableMainFreqOne->rowCount() - 1, 0,
                                      new QTableWidgetItem(FrequencyOne.at(i).first));
        ui->tableMainFreqOne->setItem(ui->tableMainFreqOne->rowCount() - 1, 1,
                                      new QTableWidgetItem(QString::number(FrequencyOne.at(i).second)));
        ui->tableMainFreqOne->setItem(ui->tableMainFreqOne->rowCount() - 1, 2,
                                      new QTableWidgetItem(formatRate(FrequencyOne.at(i).second, TimeOne)));
    }

    for (int i(0); i<DurationOne.count(); i++)
    {
        ui->tableMainDurOne->insertRow(ui->tableMainDurOne->rowCount());
        ui->tableMainDurOne->setItem(ui->tableMainDurOne->rowCount() - 1, 0,
                                      new QTableWidgetItem(DurationOne.at(i).first));
        ui->tableMainDurOne->setItem(ui->tableMainDurOne->rowCount() - 1, 1,
                                      new QTableWidgetItem(formatTimeLabel(DurationOne.at(i).second)));
        ui->tableMainDurOne->setItem(ui->tableMainDurOne->rowCount() - 1, 2,
                                      new QTableWidgetItem(formatPercentage(DurationOne.at(i).second, TimeOne)));
    }

    for (int i(0); i<FrequencyTwo.count(); i++)
    {
        ui->tableMainFreqTwo->insertRow(ui->tableMainFreqTwo->rowCount());
        ui->tableMainFreqTwo->setItem(ui->tableMainFreqTwo->rowCount() - 1, 0,
                                      new QTableWidgetItem(FrequencyTwo.at(i).first));
        ui->tableMainFreqTwo->setItem(ui->tableMainFreqTwo->rowCount() - 1, 1,
                                      new QTableWidgetItem(QString::number(FrequencyTwo.at(i).second)));
        ui->tableMainFreqTwo->setItem(ui->tableMainFreqTwo->rowCount() - 1, 2,
                                      new QTableWidgetItem(formatRate(FrequencyTwo.at(i).second, TimeTwo)));
    }

    for (int i(0); i<DurationTwo.count(); i++)
    {
        ui->tableMainDurTwo->insertRow(ui->tableMainDurTwo->rowCount());
        ui->tableMainDurTwo->setItem(ui->tableMainDurTwo->rowCount() - 1, 0,
                                      new QTableWidgetItem(DurationTwo.at(i).first));
        ui->tableMainDurTwo->setItem(ui->tableMainDurTwo->rowCount() - 1, 1,
                                      new QTableWidgetItem(formatTimeLabel(DurationTwo.at(i).second)));
        ui->tableMainDurTwo->setItem(ui->tableMainDurTwo->rowCount() - 1, 2,
                                      new QTableWidgetItem(formatPercentage(DurationTwo.at(i).second, TimeTwo)));
    }

    for (int i(0); i<FrequencyThree.count(); i++)
    {
        ui->tableMainFreqThree->insertRow(ui->tableMainFreqThree->rowCount());
        ui->tableMainFreqThree->setItem(ui->tableMainFreqThree->rowCount() - 1, 0,
                                        new QTableWidgetItem(FrequencyThree.at(i).first));
        ui->tableMainFreqThree->setItem(ui->tableMainFreqThree->rowCount() - 1, 1,
                                        new QTableWidgetItem(QString::number(FrequencyThree.at(i).second)));
        ui->tableMainFreqThree->setItem(ui->tableMainFreqThree->rowCount() - 1, 2,
                                        new QTableWidgetItem(formatRate(FrequencyThree.at(i).second,TimeThree)));
    }

    for (int i(0); i<DurationThree.count(); i++)
    {
        ui->tableMainDurThree->insertRow(ui->tableMainDurThree->rowCount());
        ui->tableMainDurThree->setItem(ui->tableMainDurThree->rowCount() - 1, 0,
                                      new QTableWidgetItem(DurationThree.at(i).first));
        ui->tableMainDurThree->setItem(ui->tableMainDurThree->rowCount() - 1, 1,
                                      new QTableWidgetItem(formatTimeLabel(DurationThree.at(i).second)));
        ui->tableMainDurThree->setItem(ui->tableMainDurThree->rowCount() - 1, 2,
                                      new QTableWidgetItem(formatPercentage(DurationThree.at(i).second, TimeThree)));
    }
}

void ResultsDialog::BuildPlot(KeySet currKeySet, QList<SessionEvent> * PressedKeys, QDateTime * startTime, QDateTime * endTime)
{
    qint64 totalMsecs = startTime->secsTo(*endTime);

    int bins = totalMsecs / 10;
    int overflow = ((totalMsecs % 10) > 0) ? 1 : 0;
        bins = bins + overflow;

    int fKeys = currKeySet.FrequencyKeys.count();
    int dKeys = currKeySet.DurationKeys.count();

    // Prepopulate bins
    QList<QList<int>> mFrequencyBins;

    for (int i(0); i<bins; i++)
    {
        mFrequencyBins.append(QList<int>());

        for (int j(0); j<fKeys; j++)
        {
            mFrequencyBins[i].append(0);
        }
    }

    QList<QList<double>> mDurationBins;

    for (int i(0); i<bins; i++)
    {
        mDurationBins.append(QList<double>());

        for (int j(0); j<dKeys; j++)
        {
            mDurationBins[i].append(0.0);
        }
    }

    KeySetEntry temp;
    int timeHolder;

    // Good, freq counts
    for (int i(0); i<fKeys; i++)
    {
        temp = currKeySet.FrequencyKeys.at(i);

        foreach(SessionEvent event, *PressedKeys)
        {
            if (event.KeyEntered.KeyCode == temp.KeyCode)
            {
                timeHolder = startTime->secsTo(event.TimePressed) / 10;

                //mFrequencyBins[i][timeHolder] = mFrequencyBins[i][timeHolder] + 1;
                mFrequencyBins[timeHolder][i] = mFrequencyBins[timeHolder][i] + 1;
            }
        }
    }

    bool waitingForNext = false;
    QDateTime prev, after;

    QList<QPair<qint64, qint64>> runs;
    qint64 start, stop;

    for (int i(0); i<dKeys; i++)
    {
        KeySetEntry temp = currKeySet.DurationKeys.at(i);
        waitingForNext = false;
        runs.clear();

        foreach(SessionEvent event, *PressedKeys)
        {
            if (event.KeyEntered.KeyCode == temp.KeyCode)
            {
                if (waitingForNext)
                {
                    after = event.TimePressed;
                    runs.append(QPair<qint64, qint64>(startTime->msecsTo(prev), startTime->msecsTo(after)));
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
            runs.append(QPair<qint64, qint64>(startTime->msecsTo(prev), startTime->msecsTo(*endTime)));
        }

        for (int j(0); j<runs.length(); j++)
        {
            start = runs[j].first;
            stop = runs[j].second;

            int index1 = (int)(start / 10000);
            int index2 = (int)(stop / 10000);

            if (index1 == index2)
            {
                //mDurationBins[i][index1] = mDurationBins[i][index1] + ((double)(stop - start))/1000;
                mDurationBins[index1][i] = mDurationBins[index1][i] + ((double)(stop - start))/1000;
            }
            else if ((index2 - index1) == 1)
            {
                //mDurationBins[i][index1] = mDurationBins[i][index1] + ((double)(10000 - (start % 10000)))/1000;
                //mDurationBins[i][index2] = mDurationBins[i][index2] + ((double)(stop % 10000))/1000;
                mDurationBins[index1][i] = mDurationBins[index1][i] + ((double)(10000 - (start % 10000)))/1000;
                mDurationBins[index1][i] = mDurationBins[index1][i] + ((double)(stop % 10000))/1000;
            }
            else
            {
                //mDurationBins[i][index1] = mDurationBins[i][index1] + ((double)(10000 - (start % 10000)))/1000;
                mDurationBins[index1][i] = mDurationBins[index1][i] + ((double)(10000 - (start % 10000)))/1000;

                for (int k(index1 + 1); k < index2; k++)
                {
                    //mDurationBins[i][k] = mDurationBins[i][k] + 10;
                    mDurationBins[k][i] = mDurationBins[k][i] + 10;
                }

                //mDurationBins[i][index2] = mDurationBins[i][index2] + ((double)(stop % 10000))/1000;
                mDurationBins[index2][i] = mDurationBins[index2][i] + ((double)(stop % 10000))/1000;
            }
        }
    }

    chart.setTitle("Cumulative Target Counts");
    chart.setTitleFont(QFont("Serif", 10, -1, false));
    chart.setTitleBrush(Qt::black);

    axisX.applyNiceNumbers();
    axisX.setGridLineColor(Qt::transparent);
    axisX.setTitleText(tr("10s Bins"));
    axisX.setTitleFont(QFont("Serif", 10, -1, false));
    axisX.setTitleBrush(Qt::black);
    //axisX.setMin(0);
    //axisX.setMin(10);
    axisX.setLabelsFont(QFont("Serif", 10, -1, false));
    axisX.setLabelFormat(QString("%.0f"));
    axisX.setLabelsBrush(Qt::black);
    axisX.setLabelsColor(Qt::black);
    axisX.setLinePenColor(Qt::black);
    axisX.setLinePen(QPen(Qt::black));

    axisY.applyNiceNumbers();
    axisY.setGridLineColor(Qt::transparent);
    axisY.setTitleText(tr("Frequency"));
    axisY.setTitleFont(QFont("Serif", 10, -1, false));
    axisY.setTitleBrush(Qt::black);
    //axisY.setTickCount(5);
    axisY.setLabelsFont(QFont("Serif", 10, -1, false));
    axisY.setLabelsBrush(Qt::black);
    axisY.setLabelsColor(Qt::black);
    axisY.setMin(0);
    axisY.setMax(100);
    axisY.setLinePenColor(Qt::black);
    axisY.setLinePen(QPen(Qt::black));

    int runTotal = 0;

    for (int i(0); i<fKeys; i++)
    {
        lineSeries.append(new QLineSeries);

        lineSeries[i]->setUseOpenGL(true);
        lineSeries[i]->setName(currKeySet.FrequencyKeys.at(i).KeyDescription);
        lineSeries[i]->clear();
        lineSeries[i]->show();

        chart.addSeries(lineSeries[i]);

        chart.setAxisX(&axisX, lineSeries[i]);
        chart.setAxisY(&axisY, lineSeries[i]);

        runTotal = 0;
    }

    chartView = new QChartView(&chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    ui->plotLayout->addWidget(chartView);

    int max = 0;

    for (int i(0); i<fKeys; i++)
    {
        runTotal = 0;

        lineSeries[i]->append(0, 0);
        for (int j(0); j<mFrequencyBins.length(); j++)
        {
            runTotal = runTotal + mFrequencyBins[j][i];
            lineSeries[i]->append(j + 1, runTotal);
        }

        if (runTotal > max)
        {
            max = runTotal;
        }
    }

    axisY.setMin(0);
    axisY.setMax(max + 1);

    axisX.setMin(0);
    axisX.setMax(mFrequencyBins.length() + 1);

    // dur

    chart2.setTitle("Cumulative Time Counts");
    chart2.setTitleFont(QFont("Serif", 10, -1, false));
    chart2.setTitleBrush(Qt::black);

    axisX2.applyNiceNumbers();
    axisX2.setGridLineColor(Qt::transparent);
    axisX2.setTitleText(tr("10s Bins"));
    axisX2.setTitleFont(QFont("Serif", 10, -1, false));
    axisX2.setTitleBrush(Qt::black);
    //axisX2.setMin(0);
    //axisX2.setMin(10);
    axisX2.setLabelsFont(QFont("Serif", 10, -1, false));
    axisX2.setLabelFormat(QString("%.0f"));
    axisX2.setLabelsBrush(Qt::black);
    axisX2.setLabelsColor(Qt::black);
    axisX2.setLinePenColor(Qt::black);
    axisX2.setLinePen(QPen(Qt::black));

    axisY2.applyNiceNumbers();
    axisY2.setGridLineColor(Qt::transparent);
    axisY2.setTitleText(tr("Time (s)"));
    axisY2.setTitleFont(QFont("Serif", 10, -1, false));
    axisY2.setTitleBrush(Qt::black);
    //axisY2.setTickCount(5);
    axisY2.setLabelsFont(QFont("Serif", 10, -1, false));
    axisY2.setLabelsBrush(Qt::black);
    axisY2.setLabelsColor(Qt::black);
    axisY2.setMin(0);
    axisY2.setMax(100);
    axisY2.setLinePenColor(Qt::black);
    axisY2.setLinePen(QPen(Qt::black));

    runTotal = 0;

    for (int i(0); i<dKeys; i++)
    {
        lineSeries2.append(new QLineSeries);

        lineSeries2[i]->setUseOpenGL(true);
        lineSeries2[i]->setName(currKeySet.DurationKeys.at(i).KeyDescription);
        lineSeries2[i]->clear();
        lineSeries2[i]->show();

        chart2.addSeries(lineSeries2[i]);

        chart2.setAxisX(&axisX2, lineSeries2[i]);
        chart2.setAxisY(&axisY2, lineSeries2[i]);

        runTotal = 0;
    }

    chartView2 = new QChartView(&chart2);
    chartView2->setRenderHint(QPainter::Antialiasing);

    ui->plotLayout2->addWidget(chartView2);

    for (int i(0); i<dKeys; i++)
    {
        runTotal = 0;

        lineSeries2[i]->append(0, 0);
        for (int j(0); j<mDurationBins.length(); j++)
        {
            runTotal = runTotal + mDurationBins[j][i];
            lineSeries2[i]->append(j + 1, runTotal);
        }

        if (runTotal > max)
        {
            max = runTotal;
        }
    }

    axisY2.setMin(0);
    axisY2.setMax(max + 1);

    axisX2.setMin(0);
    axisX2.setMax(mDurationBins.length() + 1);
}

void ResultsDialog::BuildNarrative(QList<SessionEvent> * PressedKeys, QDateTime * startTime)
{
    QStringList *mEntries = new QStringList();

    for (int i(0); i<PressedKeys->count(); i++)
    {
        mEntries->append("(" +
                         formatTimeLabel(startTime->msecsTo(PressedKeys->at(i).TimePressed)) +
                         ") " +
                         PressedKeys->at(i).KeyEntered.KeyDescription);
    }

    QStringListModel *listModel = new QStringListModel(*mEntries, NULL);
    listModel->setStringList(*mEntries);
    ui->listView->setModel(listModel);
}

void ResultsDialog::BuildJson(QList<SessionEvent> * PressedKeys, QDateTime * startTime, QString mWorkingDirectory)
{
    QJsonObject json;

    // Demographics
    json["Session"] = CurrentKeySet.Session;
    json["Group"] = Group;
    json["Individual"] = Individual;
    json["Evaluation"] = Evaluation;
    json["Condition"] = Condition;
    json["Therapist"] = Therapist;
    json["KeySet"] = KeySetName;
    json["Collector"] = Collector;
    json["Role"] = Role;
    json["StartTime"] = startTime->toString();
    json["SessionDuration"] = TimeOverall;
    json["ScheduleOneDuration"] = TimeOne;
    json["ScheduleTwoDuration"] = TimeTwo;
    json["ScheduleThreeDuration"] = TimeThree;

    // Keys
    QJsonArray frequencyKeys;
    foreach(KeySetEntry entry, CurrentKeySet.FrequencyKeys)
    {
        QJsonObject mEntry;
        mEntry["Code"] = entry.KeyCode;
        mEntry["Name"] = entry.KeyName;
        mEntry["Description"] = entry.KeyDescription;

        frequencyKeys.append(mEntry);
    }
    json["FrequencyKeys"] = frequencyKeys;

    QJsonArray durationKeys;
    foreach(KeySetEntry entry, CurrentKeySet.DurationKeys)
    {
        QJsonObject mEntry;
        mEntry["Code"] = entry.KeyCode;
        mEntry["Name"] = entry.KeyName;
        mEntry["Description"] = entry.KeyDescription;

        durationKeys.append(mEntry);
    }
    json["DurationKeys"] = durationKeys;

    QJsonArray pressedKeys;
    foreach(SessionEvent event, *PressedKeys)
    {
        QJsonObject mEntry;
        mEntry["KeyCode"] = event.KeyEntered.KeyCode;
        mEntry["KeyName"] = event.KeyEntered.KeyName;
        mEntry["KeyDescription"] = event.KeyEntered.KeyDescription;
        mEntry["TimePressed"] = event.TimePressed.toString();
        mEntry["Schedule"] = formatSchedule(event.ScheduleType);
        mEntry["Measurement"] = formatMeasurement(event.MeasurementType);

        pressedKeys.append(mEntry);
    }
    json["PressedKeys"] = pressedKeys;

    QJsonDocument jsonDoc(json);

    QString mKeyPath = FileTools::pathAppend(mWorkingDirectory, Group);
    mKeyPath = FileTools::pathAppend(mKeyPath, Individual);
    mKeyPath = FileTools::pathAppend(mKeyPath, Evaluation);
    mKeyPath = FileTools::pathAppend(mKeyPath, Condition);

    QString mFileName = QString("%1%2%3%4_%5.json")
            .arg(QString::number(CurrentKeySet.Session).rightJustified(3, '0'))
            .arg(Group.mid(0, 3))
            .arg(Individual.mid(0, 3))
            .arg(Evaluation.mid(0, 3))
            .arg(Role.mid(0, 1));

    QString path = FileTools::pathAppend(mKeyPath, mFileName);

    QFile saveFile(path);

    if (!saveFile.open(QIODevice::WriteOnly)) {
        // TODO error handling
        return;
    }

    saveFile.write(jsonDoc.toJson());

}

ResultsDialog::~ResultsDialog()
{
    delete ui;
}
