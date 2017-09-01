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

#include "resultsdialog.h"
#include "ui_resultsdialog.h"

#include "sessionevent.h"
#include "filetools.h"

#include <QStringListModel>
#include <QJsonObject>
#include <QJsonArray>
#include <QFile>
#include <QPixmap>

ResultsDialog::ResultsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResultsDialog)
{
    ui->setupUi(this);

    Qt::WindowFlags windowFlags = 0;
    windowFlags |= Qt::WindowMaximizeButtonHint;
    windowFlags |= Qt::WindowMinimizeButtonHint;
    windowFlags |= Qt::WindowCloseButtonHint;

    setWindowFlags(windowFlags);

    setWindowTitle(tr("Session Results"));
}

/** Set up results window with information
 * @brief ResultsDialog::SetParameters
 * @param group
 * @param individual
 * @param evaluation
 * @param condition
 * @param therapist
 * @param keySet
 * @param collector
 * @param role
 */
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

    ui->labelTitle->setText(QString("<html><head/><body><p align='center'><span style=' font-size:12pt;'>Recording: Session #%1</span></p></body></html>")
                            .arg(CurrentKeySet.Session));
}

/**
 * @brief ResultsDialog::SetTabEnabled
 * @param index
 * @param value
 */
void ResultsDialog::SetTabEnabled(int index, bool value)
{
    ui->tabWidget_2->setTabEnabled(index, value);
}

/** Assign key set
 * @brief ResultsDialog::SetKeySet
 * @param currKeySet
 */
void ResultsDialog::SetKeySet(KeySet currKeySet)
{
    CurrentKeySet = currKeySet;
}

/**
 * @brief ResultsDialog::SetTitle
 * @param title
 */
void ResultsDialog::SetTitle(QString title)
{
    ui->labelTitle->setText(QString("<html><head/><body><p align='center'><span style=' font-size:12pt;'>#%1</span></p></body></html>")
                            .arg(title));
}

/** Build display tables
 * @brief ResultsDialog::BuildTables
 */
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

/** Build plots for display
 * @brief ResultsDialog::BuildPlot
 * @param currKeySet
 * @param PressedKeys
 * @param startTime
 * @param endTime
 */
void ResultsDialog::BuildPlot(KeySet currKeySet, QList<SessionEvent> * PressedKeys, QDateTime * startTime, QDateTime * endTime)
{
    lineSeries.clear();
    lineSeries2.clear();

    qint64 totalSecs = startTime->secsTo(*endTime),
           secs;

    QStringList fKeyList,
                dKeyList;
    QList<double> fKeySum;
    QList<bool> dKeyRunning;

    int max = 0,
        startSecs,
        endSecs;

    double runningSum;

    QString tempKey;

    for (int i(0); i<currKeySet.FrequencyKeys.count(); i++)
    {
        fKeyList.append(currKeySet.FrequencyKeys.at(i).KeyDescription);
        fKeySum.append(0.0);
    }

    for (int i(0); i<currKeySet.DurationKeys.count(); i++)
    {
        dKeyList.append(currKeySet.DurationKeys.at(i).KeyDescription);
        dKeyRunning.append(false);
    }

    bool waitingForNext = false;
    QDateTime prev, after;

    chart = new QChart();
    chart->layout()->setContentsMargins(0, 0, 0, 0);
    chart->setBackgroundRoundness(0);
    chart->setTitle("Cumulative Target Counts");
    chart->setTitleFont(QFont("Serif", 10, -1, false));
    chart->setTitleBrush(Qt::black);

    axisX = new QValueAxis;
    axisX->applyNiceNumbers();
    axisX->setGridLineColor(Qt::transparent);
    axisX->setTitleText(tr("Session Time (seconds)"));
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

    for (int i(0); i<fKeyList.count(); i++)
    {
        lineSeries.append(new QLineSeries);

        lineSeries[i]->setUseOpenGL(true);
        lineSeries[i]->setName(fKeyList.at(i));
        lineSeries[i]->clear();
        lineSeries[i]->show();

        chart->addSeries(lineSeries[i]);

        chart->setAxisX(axisX, lineSeries[i]);
        chart->setAxisY(axisY, lineSeries[i]);
    }

    chartView = new QChartView(chart);
    chartView->setRenderHint(QPainter::Antialiasing);

    if (!Drawn)
    {
        ui->plotLayout->addWidget(chartView);
    }

    for (int i(0); i<fKeyList.count(); i++)
    {
        tempKey = fKeyList.at(i);
        fKeySum[i] = 0;

        foreach (SessionEvent key, *PressedKeys) {
            if (key.KeyEntered.KeyDescription == tempKey)
            {
                int fIndex = fKeyList.indexOf(key.KeyEntered.KeyDescription);

                if (fIndex != -1)
                {
                    secs = startTime->secsTo(key.TimePressed);

                    *lineSeries[fKeyList.indexOf(tempKey)] << QPointF(secs, fKeySum[fKeyList.indexOf(tempKey)]);
                    fKeySum[fKeyList.indexOf(tempKey)] = fKeySum[fKeyList.indexOf(tempKey)] + 1;
                    *lineSeries[fKeyList.indexOf(tempKey)] << QPointF(secs, fKeySum[fKeyList.indexOf(tempKey)]);

                    if (fKeySum[fKeyList.indexOf(tempKey)] > (int) max)
                    {
                        max = (double) fKeySum[fKeyList.indexOf(tempKey)];
                    }
                }
            }
        }
    }

    // Cap off frequency
    for (int i(0); i<lineSeries.count(); i++)
    {
        lineSeries.at(i)->append(totalSecs, lineSeries.at(i)->at(lineSeries.at(i)->count() - 1).y());
    }

    axisY->setMin(0);
    axisY->setMax(max + 1);

    axisX->setMin(0);
    axisX->setMax(startTime->secsTo(*endTime));

    // Duration plotting

    chart2 = new QChart();
    chart2->layout()->setContentsMargins(0, 0, 0, 0);
    chart2->setBackgroundRoundness(0);
    chart2->setTitle("Cumulative Time Counts");
    chart2->setTitleFont(QFont("Serif", 10, -1, false));
    chart2->setTitleBrush(Qt::black);

    axisX2 = new QValueAxis;
    axisX2->applyNiceNumbers();
    axisX2->setGridLineColor(Qt::transparent);
    axisX2->setTitleText(tr("10s Bins"));
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
    axisY2->setTitleText(tr("Time (s)"));
    axisY2->setTitleFont(QFont("Serif", 10, -1, false));
    axisY2->setTitleBrush(Qt::black);
    axisY2->setLabelsFont(QFont("Serif", 10, -1, false));
    axisY2->setLabelsBrush(Qt::black);
    axisY2->setLabelsColor(Qt::black);
    axisY2->setMin(0);
    axisY2->setMax(100);
    axisY2->setLinePenColor(Qt::black);
    axisY2->setLinePen(QPen(Qt::black));

    for (int i(0); i<dKeyList.count(); i++)
    {
        lineSeries2.append(new QLineSeries);

        lineSeries2[i]->setUseOpenGL(true);
        lineSeries2[i]->setName(dKeyList.at(i));
        lineSeries2[i]->clear();
        lineSeries2[i]->show();

        chart2->addSeries(lineSeries2[i]);

        chart2->setAxisX(axisX2, lineSeries2[i]);
        chart2->setAxisY(axisY2, lineSeries2[i]);
    }

    for (int i(0); i<dKeyList.count(); i++)
    {
        tempKey = dKeyList.at(i);
        waitingForNext = false;
        runningSum = 0;

        foreach (SessionEvent key, *PressedKeys) {
            if (key.KeyEntered.KeyDescription == tempKey)
            {
                if (waitingForNext)
                {
                    after = key.TimePressed;

                    startSecs = ((double) startTime->msecsTo(prev)) / 1000;
                    endSecs = ((double) startTime->msecsTo(after)) / 1000;

                    *lineSeries2[dKeyList.indexOf(tempKey)] << QPointF(startSecs, runningSum);
                    runningSum = runningSum + (endSecs - startSecs);
                    *lineSeries2[dKeyList.indexOf(tempKey)] << QPointF(endSecs, runningSum);

                    if (runningSum > (int) max)
                    {
                        max = (double) runningSum;
                    }

                    waitingForNext = false;
                }
                else
                {
                    prev = key.TimePressed;
                    waitingForNext = true;
                }
            }
        }

        if (waitingForNext)
        {
            startSecs = ((double) startTime->msecsTo(prev)) / 1000;

            *lineSeries2[dKeyList.indexOf(tempKey)] << QPointF(startSecs, runningSum);
            runningSum = runningSum + (totalSecs - startSecs);
            *lineSeries2[dKeyList.indexOf(tempKey)] << QPointF(totalSecs, runningSum);
        }

        if ((int) runningSum > max)
        {
            max = (int) runningSum;
        }
    }

    // Cap off duration
    for (int i(0); i<lineSeries2.count(); i++)
    {
        lineSeries2.at(i)->append(totalSecs, lineSeries2.at(i)->at(lineSeries2.at(i)->count() - 1).y());
    }

    axisY2->setMax(max + 1);
    axisX2->setMax(totalSecs);

    chartView2 = new QChartView(chart2);
    chartView2->setRenderHint(QPainter::Antialiasing);

    if (!Drawn)
    {
        ui->plotLayout2->addWidget(chartView2);
    }

    Drawn = true;
}

/** Build log output
 * @brief ResultsDialog::BuildNarrative
 * @param PressedKeys
 * @param startTime
 */
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

ResultsDialog::~ResultsDialog()
{
    delete ui;
}
