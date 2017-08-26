#include "sessionwindow.h"
#include "ui_sessionwindow.h"

#include "filetools.h"
#include "parsetypes.h"
#include "keyseteditor.h"
#include "keysetentry.h"

#include <QtWidgets>

SessionWindow::SessionWindow(QString mCurrentWorkingDirectory, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SessionWindow)
{
    ui->setupUi(this);

    mWorkingDirectory = mCurrentWorkingDirectory;

    setWindowTitle(tr("Session Designer"));

    workerThread = new QThread();

    mCurrentDirectory.WorkingDirectory = mWorkingDirectory;

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

/** Add a new group
 * @brief SessionWindow::on_buttonGroup_clicked
 */
void SessionWindow::on_buttonGroup_clicked()
{
    bool ok = false;
    QString text = QInputDialog::getText(this, tr("Enter New Group"), tr("Group Name:"), QLineEdit::Normal, "", &ok);

    if (ok)
    {
        if (ui->comboGroup->findText(text) == -1)
        {
            QString mPath = FileTools::pathAppend(mWorkingDirectory, text);
            QDir mPresentDirectory(mPath);

            qDebug() << mPresentDirectory;

            if (!mPresentDirectory.exists()){
                mPresentDirectory.mkdir(".");
            }

            workerThread = new QThread();

            mCurrentDirectory.WorkingDirectory = mWorkingDirectory;
            mCurrentDirectory.CurrentGroup = "";
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
    }
}

/** A group is selected
 * @brief SessionWindow::on_comboGroup_currentIndexChanged
 * @param index
 */
void SessionWindow::on_comboGroup_currentIndexChanged(int index)
{
    workerThread = new QThread();

    mCurrentDirectory.WorkingDirectory = mWorkingDirectory;
    mCurrentDirectory.CurrentGroup = ui->comboGroup->itemText(index);
    mCurrentDirectory.CurrentIndividual = "";
    mCurrentDirectory.CurrentEvaluation = "";
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

/** Add a new individual
 * @brief SessionWindow::on_buttonIndividual_clicked
 */
void SessionWindow::on_buttonIndividual_clicked()
{
    if (ui->comboGroup->currentIndex() == 0)
    {
        return;
    }

    bool ok = false;
    QString text = QInputDialog::getText(this, tr("Enter New Individual"), tr("Individual Name:"), QLineEdit::Normal, "", &ok);

    if (ok)
    {
        if (ui->comboIndividual->findText(text) == -1)
        {
            QString mPath = FileTools::pathAppend(mWorkingDirectory, ui->comboGroup->currentText());
            mPath = FileTools::pathAppend(mPath, text);

            QDir mPresentDirectory(mPath);

            if (!mPresentDirectory.exists()){
                mPresentDirectory.mkdir(".");
            }

            workerThread = new QThread();

            mCurrentDirectory.WorkingDirectory = mWorkingDirectory;
            mCurrentDirectory.CurrentGroup = ui->comboGroup->currentText();
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
    }
}

/** An individual is selected
 * @brief SessionWindow::on_comboIndividual_currentIndexChanged
 * @param index
 */
void SessionWindow::on_comboIndividual_currentIndexChanged(int index)
{
    workerThread = new QThread();

    mCurrentDirectory.WorkingDirectory = mWorkingDirectory;
    mCurrentDirectory.CurrentGroup = ui->comboGroup->currentText();
    mCurrentDirectory.CurrentIndividual = ui->comboIndividual->itemText(index);
    mCurrentDirectory.CurrentEvaluation = "";
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

/** Add a new evaluation
 * @brief SessionWindow::on_buttonEvaluation_clicked
 */
void SessionWindow::on_buttonEvaluation_clicked()
{
    if (ui->comboGroup->currentIndex() == 0 || ui->comboIndividual->currentIndex() == 0 )
    {
        return;
    }

    bool ok = false;
    QString text = QInputDialog::getText(this, tr("Enter New Evaluation"), tr("New Evaluation:"), QLineEdit::Normal, "", &ok);

    if (ok)
    {
        if (ui->comboEvaluation->findText(text) == -1)
        {
            QString mPath = FileTools::pathAppend(mWorkingDirectory, ui->comboGroup->currentText());
            mPath = FileTools::pathAppend(mPath, ui->comboIndividual->currentText());
            mPath = FileTools::pathAppend(mPath, text);

            QDir mPresentDirectory(mPath);

            if (!mPresentDirectory.exists()){
                mPresentDirectory.mkdir(".");
            }

            workerThread = new QThread();

            mCurrentDirectory.WorkingDirectory = mWorkingDirectory;
            mCurrentDirectory.CurrentGroup = ui->comboGroup->currentText();
            mCurrentDirectory.CurrentIndividual = ui->comboIndividual->currentText();
            mCurrentDirectory.CurrentEvaluation = "";
            mCurrentDirectory.CurrentCondition = "";

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
    }
}

/** An evaluation is selected
 * @brief SessionWindow::on_comboEvaluation_currentIndexChanged
 * @param index
 */
void SessionWindow::on_comboEvaluation_currentIndexChanged(int index)
{
    workerThread = new QThread();

    mCurrentDirectory.WorkingDirectory = mWorkingDirectory;
    mCurrentDirectory.CurrentGroup = ui->comboGroup->currentText();
    mCurrentDirectory.CurrentIndividual = ui->comboIndividual->currentText();
    mCurrentDirectory.CurrentEvaluation = ui->comboEvaluation->itemText(index);
    mCurrentDirectory.CurrentCondition = "";

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

/** Add a new condition
 * @brief SessionWindow::on_buttonCondition_clicked
 */
void SessionWindow::on_buttonCondition_clicked()
{
    if (ui->comboGroup->currentIndex() == 0
            || ui->comboIndividual->currentIndex() == 0
            || ui->comboEvaluation->currentIndex() == 0 )
    {
        return;
    }

    bool ok = false;
    QString text = QInputDialog::getText(this, tr("Enter New Condition"), tr("New Condition:"), QLineEdit::Normal, "", &ok);

    if (ok)
    {
        if (ui->comboCondition->findText(text) == -1)
        {
            QString mPath = FileTools::pathAppend(mWorkingDirectory, ui->comboGroup->currentText());
            mPath = FileTools::pathAppend(mPath, ui->comboIndividual->currentText());
            mPath = FileTools::pathAppend(mPath, ui->comboEvaluation->currentText());
            mPath = FileTools::pathAppend(mPath, text);

            QDir mPresentDirectory(mPath);

            if (!mPresentDirectory.exists()){
                mPresentDirectory.mkdir(".");
            }

            workerThread = new QThread();

            mCurrentDirectory.WorkingDirectory = mWorkingDirectory;
            mCurrentDirectory.CurrentGroup = ui->comboGroup->currentText();
            mCurrentDirectory.CurrentIndividual = ui->comboIndividual->currentText();
            mCurrentDirectory.CurrentEvaluation = ui->comboEvaluation->currentText();
            mCurrentDirectory.CurrentCondition = "";

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
    }
}

/** Add/edit a key set
 * @brief SessionWindow::on_buttonKeySet_clicked
 */
void SessionWindow::on_buttonKeySet_clicked()
{
    if (ui->comboGroup->currentIndex() == 0
            || ui->comboIndividual->currentIndex() == 0)
    {
        return;
    }

    bool ok;
    QStringList listOptions;
    listOptions << tr("Add KeySet") << tr("Edit KeySet");

    if (ui->comboKeySet->currentIndex() == 0)
    {
        QString keySetName = QInputDialog::getText(this, tr("Name the new KeySet"),
                                                   tr("Key Set Name:"),
                                                   QLineEdit::Normal,
                                                   "", &ok);
        if (ok && !keySetName.isEmpty())
        {
            KeySetEditor mKeySetEntry;
            mKeySetEntry.KeySetName = keySetName;

            mKeySetEntry.exec();

            QString mKeyPath = FileTools::pathAppend(mWorkingDirectory, ui->comboGroup->currentText());
            mKeyPath = FileTools::pathAppend(mKeyPath, ui->comboIndividual->currentText());

            FileTools::WriteKeySet(FileTools::pathAppend(mKeyPath, QString("%1.json").arg(keySetName)), mKeySetEntry.keySet);

        }
    }
    else
    {
        QString selectedOption = QInputDialog::getItem(this, tr("Add or Edit Key Set?"),
                                                       tr("Pick option:"), listOptions, 0, false, &ok);
        if (ok && !selectedOption.isEmpty())
        {
            int mSelection = listOptions.indexOf(selectedOption);

            if (mSelection == 0)
            {
                QString keySetName = QInputDialog::getText(this, tr("Name the new KeySet"),
                                                           tr("Key Set Name:"),
                                                           QLineEdit::Normal,
                                                           "", &ok);
                if (ok && !keySetName.isEmpty())
                {
                    KeySetEditor mKeySetEntry;
                    mKeySetEntry.KeySetName = keySetName;

                    mKeySetEntry.exec();

                    QString mKeyPath = FileTools::pathAppend(mWorkingDirectory, ui->comboGroup->currentText());
                    mKeyPath = FileTools::pathAppend(mKeyPath, ui->comboIndividual->currentText());

                    FileTools::WriteKeySet(FileTools::pathAppend(mKeyPath, QString("%1.json").arg(keySetName)), mKeySetEntry.keySet);

                }
            }
            else if (mSelection == 1)
            {
                KeySetEditor mKeySetEntry;
                mKeySetEntry.KeySetName = ui->comboKeySet->currentText();

                mKeySetEntry.loadExistingKeys(mWorkingDirectory, ui->comboGroup->currentText(), ui->comboIndividual->currentText());

                mKeySetEntry.exec();

                QString mKeyPath = FileTools::pathAppend(mWorkingDirectory, ui->comboGroup->currentText());
                mKeyPath = FileTools::pathAppend(mKeyPath, ui->comboIndividual->currentText());

                FileTools::WriteKeySet(FileTools::pathAppend(mKeyPath, QString("%1.json").arg(ui->comboKeySet->currentText())), mKeySetEntry.keySet);

                ui->tableFrequency->setRowCount(0);
                ui->tableDuration->setRowCount(0);

                CurrentKeySet.KeySetName = "";
                CurrentKeySet.DurationKeys.clear();
                CurrentKeySet.FrequencyKeys.clear();

                FileTools::ReadKeySet(FileTools::pathAppend(mKeyPath, QString("%1.json").arg(ui->comboKeySet->currentText())), &CurrentKeySet);

                ui->tableFrequency->setRowCount(0);

                for (KeySetEntry mKey : CurrentKeySet.FrequencyKeys)
                {
                    ui->tableFrequency->insertRow(ui->tableFrequency->rowCount());
                    ui->tableFrequency->setItem(ui->tableFrequency->rowCount() - 1, 0, new QTableWidgetItem(mKey.KeyName));
                    ui->tableFrequency->setItem(ui->tableFrequency->rowCount() - 1, 1, new QTableWidgetItem(mKey.KeyDescription));
                }

                ui->tableDuration->setRowCount(0);

                for (KeySetEntry mKey : CurrentKeySet.DurationKeys)
                {
                    ui->tableDuration->insertRow(ui->tableDuration->rowCount());
                    ui->tableDuration->setItem(ui->tableDuration->rowCount() - 1, 0, new QTableWidgetItem(mKey.KeyName));
                    ui->tableDuration->setItem(ui->tableDuration->rowCount() - 1, 1, new QTableWidgetItem(mKey.KeyDescription));
                }

            }
        }
    }

}

/**
 * @brief SessionWindow::on_comboKeySet_currentIndexChanged
 * @param index
 */
void SessionWindow::on_comboKeySet_currentIndexChanged(int index)
{
    ui->tableFrequency->setRowCount(0);
    ui->tableDuration->setRowCount(0);

    CurrentKeySet.KeySetName = "";
    CurrentKeySet.DurationKeys.clear();
    CurrentKeySet.FrequencyKeys.clear();

    if (ui->comboGroup->currentIndex() == 0 ||
        ui->comboIndividual->currentIndex() == 0 ||
        ui->comboKeySet->currentIndex() == 0)
    {
        return;
    }

    QString mKeyPath = FileTools::pathAppend(mWorkingDirectory, ui->comboGroup->currentText());
    mKeyPath = FileTools::pathAppend(mKeyPath, ui->comboIndividual->currentText());

    FileTools::ReadKeySet(FileTools::pathAppend(mKeyPath, QString("%1.json").arg(ui->comboKeySet->itemText(index))), &CurrentKeySet);

    ui->tableFrequency->setRowCount(0);

    for (KeySetEntry mKey : CurrentKeySet.FrequencyKeys)
    {
        ui->tableFrequency->insertRow(ui->tableFrequency->rowCount());
        ui->tableFrequency->setItem(ui->tableFrequency->rowCount() - 1, 0, new QTableWidgetItem(mKey.KeyName));
        ui->tableFrequency->setItem(ui->tableFrequency->rowCount() - 1, 1, new QTableWidgetItem(mKey.KeyDescription));
    }

    ui->tableDuration->setRowCount(0);

    for (KeySetEntry mKey : CurrentKeySet.DurationKeys)
    {
        ui->tableDuration->insertRow(ui->tableDuration->rowCount());
        ui->tableDuration->setItem(ui->tableDuration->rowCount() - 1, 0, new QTableWidgetItem(mKey.KeyName));
        ui->tableDuration->setItem(ui->tableDuration->rowCount() - 1, 1, new QTableWidgetItem(mKey.KeyDescription));
    }
}

/** Add a new therapist
 * @brief SessionWindow::on_buttonTherapist_clicked
 */
void SessionWindow::on_buttonTherapist_clicked()
{
    if (ui->comboGroup->currentIndex() == 0
            || ui->comboIndividual->currentIndex() == 0)
    {
        return;
    }

    bool ok;
    QString therapistName = QInputDialog::getText(this, tr("Name the new therapist"),
                                               tr("Therapist Name:"),
                                               QLineEdit::Normal,
                                               "", &ok);

    if (ok && !therapistName.isEmpty())
    {
        ui->comboTherapist->addItem(therapistName);

        QStringList therapistList;

        for (int i=1; i<ui->comboTherapist->count(); i++)
        {
            therapistList << ui->comboTherapist->itemText(i);
        }

        QString mTherapistPath = FileTools::pathAppend(mWorkingDirectory, ui->comboGroup->currentText());
        mTherapistPath = FileTools::pathAppend(mTherapistPath, ui->comboIndividual->currentText());
        mTherapistPath = FileTools::pathAppend(mTherapistPath, "Therapists.json");

        FileTools::WriteTherapists(mTherapistPath, therapistList);
    }
}

/** Add a new collector
 * @brief SessionWindow::on_buttonCollector_clicked
 */
void SessionWindow::on_buttonCollector_clicked()
{
    if (ui->comboGroup->currentIndex() == 0
            || ui->comboIndividual->currentIndex() == 0)
    {
        return;
    }

    bool ok;
    QString collectorName = QInputDialog::getText(this, tr("Name the new collector"),
                                                  tr("Collector Name:"),
                                                  QLineEdit::Normal,
                                                  "", &ok);

    if (ok && !collectorName.isEmpty())
    {
        ui->comboCollector->addItem(collectorName);

        QStringList collectorList;

        for (int i=1; i<ui->comboCollector->count(); i++)
        {
            collectorList << ui->comboCollector->itemText(i);
        }

        QString mCollectorPath = FileTools::pathAppend(mWorkingDirectory, ui->comboGroup->currentText());
        mCollectorPath = FileTools::pathAppend(mCollectorPath, ui->comboIndividual->currentText());
        mCollectorPath = FileTools::pathAppend(mCollectorPath, "Collectors.json");

        FileTools::WriteCollectors(mCollectorPath, collectorList);
    }
}

/**
 * @brief SessionWindow::WorkUpdate
 * @param update
 */
void SessionWindow::WorkUpdate(QString update)
{
    qDebug() << "WORK_UPDATE: " << update;
}

/**
 * @brief SessionWindow::WorkFinished
 * @param finalResult
 * @param action
 */
void SessionWindow::WorkFinished(DirectoryParse finalResult, ParseTypes::ParseAction action)
{
    if (action == ParseTypes::Group)
    {
        SetGroups(finalResult.Groups);
    }
    else if (action == ParseTypes::Individual)
    {
        SetIndividuals(finalResult.Individuals);
    }
    else if (action == ParseTypes::Evaluation)
    {
        SetEvaluations(finalResult.Evaluations, finalResult.KeySets, finalResult.Therapists, finalResult.Collectors);
    }
    else if (action == ParseTypes::Condition)
    {
        SetConditions(finalResult.Conditions);
    }
}

/**
 * @brief SessionWindow::DefaultComboBox
 * @param mSelectedBox
 */
void SessionWindow::DefaultComboBox(QComboBox *mSelectedBox)
{
    while (mSelectedBox->count() > 1)
    {
        mSelectedBox->removeItem(1);
    }
}

/**
 * @brief SessionWindow::SetGroups
 * @param Groups
 */
void SessionWindow::SetGroups(QStringList Groups)
{
    DefaultComboBox(ui->comboGroup);
        ui->comboGroup->addItems(Groups);

    DefaultComboBox(ui->comboIndividual);
        ui->comboIndividual->setCurrentIndex(0);

    DefaultComboBox(ui->comboEvaluation);
        ui->comboEvaluation->setCurrentIndex(0);

    DefaultComboBox(ui->comboCondition);
        ui->comboCondition->setCurrentIndex(0);

    DefaultComboBox(ui->comboKeySet);
        ui->comboKeySet->setCurrentIndex(0);

    DefaultComboBox(ui->comboTherapist);
        ui->comboTherapist->setCurrentIndex(0);

    DefaultComboBox(ui->comboCollector);
        ui->comboCollector->setCurrentIndex(0);
}

/**
 * @brief SessionWindow::SetIndividuals
 * @param Individuals
 */
void SessionWindow::SetIndividuals(QStringList Individuals)
{
    DefaultComboBox(ui->comboIndividual);
        ui->comboIndividual->addItems(Individuals);

    DefaultComboBox(ui->comboEvaluation);
        ui->comboEvaluation->setCurrentIndex(0);

    DefaultComboBox(ui->comboCondition);
        ui->comboCondition->setCurrentIndex(0);

    DefaultComboBox(ui->comboKeySet);
        ui->comboKeySet->setCurrentIndex(0);

    DefaultComboBox(ui->comboTherapist);
        ui->comboTherapist->setCurrentIndex(0);

    DefaultComboBox(ui->comboCollector);
        ui->comboCollector->setCurrentIndex(0);
}

/**
 * @brief SessionWindow::SetEvaluations
 * @param Evaluations
 * @param KeySets
 * @param Therapists
 */
void SessionWindow::SetEvaluations(QStringList Evaluations, QStringList KeySets, QStringList Therapists, QStringList Collectors)
{
    DefaultComboBox(ui->comboEvaluation);
        ui->comboEvaluation->addItems(Evaluations);

    DefaultComboBox(ui->comboKeySet);
        ui->comboKeySet->addItems(KeySets);

    DefaultComboBox(ui->comboTherapist);
        ui->comboTherapist->addItems(Therapists);

    DefaultComboBox(ui->comboCollector);
        ui->comboCollector->addItems(Collectors);

    DefaultComboBox(ui->comboCondition);
        ui->comboCondition->setCurrentIndex(0);
}

/**
 * @brief SessionWindow::SetConditions
 * @param Conditions
 */
void SessionWindow::SetConditions(QStringList Conditions)
{
    DefaultComboBox(ui->comboCondition);
        ui->comboCondition->addItems(Conditions);
}

SessionWindow::~SessionWindow()
{
    delete ui;
}

void SessionWindow::on_buttonBox_clicked(QAbstractButton *button)
{
    if((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Ok))
    {
        CurrentKeySet.TotalSeconds = 60;
        CurrentKeySet.Session = 1;

        r.LoadKeys(CurrentKeySet);
        r.SetGroup(ui->comboGroup->currentText());
        r.SetIndividual(ui->comboIndividual->currentText());
        r.SetEvaluation(ui->comboEvaluation->currentText());
        r.SetCondition(ui->comboCondition->currentText());
        r.SetCollector(ui->comboCollector->currentText());
        r.SetRole(ui->comboRole->currentText());
        r.exec();


        // Scoring here
    }
}
