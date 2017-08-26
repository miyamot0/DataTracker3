#include "resultsdialog.h"
#include "ui_resultsdialog.h"

ResultsDialog::ResultsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ResultsDialog)
{
    ui->setupUi(this);
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

ResultsDialog::~ResultsDialog()
{
    delete ui;
}
