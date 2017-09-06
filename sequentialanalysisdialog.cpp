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
    }
}

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
    }

}

void SequentialAnalysisDialog::on_comboBoxEvaluation_currentIndexChanged(int index)
{
    if (index == 0)
    {
        ui->tableWidget->setRowCount(0);

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
        }
    }
}

void SequentialAnalysisDialog::on_comboBoxAnalysis_currentIndexChanged(int index)
{
    if (index == 0)
    {
        ui->comboBoxWindow->setEnabled(false);
        ui->comboBoxWindow->setCurrentIndex(0);

    }
    else if (index == 1)
    {
        ui->comboBoxWindow->setEnabled(false);
        ui->comboBoxWindow->setCurrentIndex(0);

        GetKeys();

        mSeriesSelect.setWindowTitle(tr("Select Keys to Analyze"));
        mSeriesSelect.AddOptions(keyList);

        if (mSeriesSelect.exec() != QDialog::Rejected)
        {
            keyShowList = mSeriesSelect.GetBoolList();

            ChartOddsRatio(0);
        }

    }
    else
    {
        ui->comboBoxWindow->setEnabled(true);
        ui->comboBoxWindow->setCurrentIndex(0);
    }
}

void SequentialAnalysisDialog::on_comboBoxWindow_currentIndexChanged(int index)
{
    if (index == 1)
    {
        GetKeys();

        mSeriesSelect.setWindowTitle(tr("Select Keys to Analyze"));
        mSeriesSelect.AddOptions(keyList);

        if (mSeriesSelect.exec() != QDialog::Rejected)
        {
            keyShowList = mSeriesSelect.GetBoolList();

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

void SequentialAnalysisDialog::GetKeys()
{
    QJsonArray keyArray;

    if (PrimaryReliabilityObjects.count() > 0 && FileTools::ReadSessionFromJSON(PrimaryReliabilityObjects.at(0).PrimaryFilePath, &json))
    {
        keyList.clear();

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
                    }
                }

                keyArray = json["DurationKeys"].toArray();
                foreach (const QJsonValue collector, keyArray) {
                    QJsonObject mObj = collector.toObject();

                    if (!keyList.contains((mObj["Description"].toString())))
                    {
                        keyList.append((mObj["Description"].toString()));
                    }
                }
            }
        }
    }
}

void SequentialAnalysisDialog::CalculateStats()
{
    qDebug() << keyList << " " << keyShowList;

    // Test


    //FileTools::CreateContingencyTables(temp.PrimaryFilePath, 0, 1, 0);

}

void SequentialAnalysisDialog::ChartOddsRatio(int index)
{
    temp = PrimaryReliabilityObjects.at(index);
    result = FileTools::ReadSessionFromJSON(temp.PrimaryFilePath, &json);

    QList<QStringList> mResults;
    mResults.clear();

    FileTools::CreateContingencyTables(temp.PrimaryFilePath, keyList, &mResults, 0, 1, 4);

    //qDebug() << mResults;

    ui->tableWidgetOutputs->setRowCount(0);
    ui->tableWidgetOutputs->setColumnCount(0);

    for (int i(0); i<keyList.count(); i++)
    {
        ui->tableWidgetOutputs->insertRow(ui->tableWidgetOutputs->rowCount());
        ui->tableWidgetOutputs->setVerticalHeaderItem(i, new QTableWidgetItem(keyList.at(i)));

        for (int j(0); j<keyList.count(); j++)
        {
            if (i == 0)
            {
                ui->tableWidgetOutputs->insertColumn(ui->tableWidgetOutputs->columnCount());
                ui->tableWidgetOutputs->setHorizontalHeaderItem(j, new QTableWidgetItem(keyList.at(j)));
            }

            ui->tableWidgetOutputs->setItem(i, j, new QTableWidgetItem(mResults.at(i).at(j)));
        }
    }
}

void SequentialAnalysisDialog::ChartYule(int index)
{
    temp = PrimaryReliabilityObjects.at(index);
    result = FileTools::ReadSessionFromJSON(temp.PrimaryFilePath, &json);

    QList<QStringList> mResults;
    mResults.clear();

    FileTools::CreateContingencyTables(temp.PrimaryFilePath, keyList, &mResults, 1, 1, 4);

    qDebug() << mResults;

    ui->tableWidgetOutputs->setRowCount(0);
    ui->tableWidgetOutputs->setColumnCount(0);

    for (int i(0); i<keyList.count(); i++)
    {
        ui->tableWidgetOutputs->insertRow(ui->tableWidgetOutputs->rowCount());
        ui->tableWidgetOutputs->setVerticalHeaderItem(i, new QTableWidgetItem(keyList.at(i)));

        for (int j(0); j<keyList.count(); j++)
        {
            if (i == 0)
            {
                ui->tableWidgetOutputs->insertColumn(ui->tableWidgetOutputs->columnCount());
                ui->tableWidgetOutputs->setHorizontalHeaderItem(j, new QTableWidgetItem(keyList.at(j)));
            }

            ui->tableWidgetOutputs->setItem(i, j, new QTableWidgetItem(mResults.at(i).at(j)));
        }
    }
}

void SequentialAnalysisDialog::on_tableWidget_currentCellChanged(int rowChanged, int, int, int)
{
    if (ui->comboBoxAnalysis->currentIndex() == 0)
    {
        return;
    }
    else if (ui->comboBoxAnalysis->currentIndex() == 1)
    {
        ChartOddsRatio(rowChanged);
    }
    else
    {
        ChartYule(rowChanged);
    }
}
