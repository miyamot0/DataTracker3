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

#include "sessionwindow.h"
#include "ui_sessionwindow.h"

#include "filetools.h"
#include "windowtools.h"
#include "scoringtools.h"
#include "reliabilityscoring.h"
#include "parsetypes.h"
#include "keyseteditor.h"
#include "keysetentry.h"
#include "filewriter.h"

#include <QtWidgets>
#include <QDebug>
#include <QSettings>

SessionWindow::SessionWindow(QString mCurrentWorkingDirectory, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SessionWindow)
{
    ui->setupUi(this);

    setWindowTitle(tr("Session Designer"));

    WindowTools::SetDialogFixedDisplay(this);

    mWorkingDirectory = mCurrentWorkingDirectory;

    PrepareDirectory();

    LoadSettings();

    ui->editSaveLocation->setText(mWorkingDirectory);
    ui->editAlternativeSaveLocation->setText(alternativeSaveLocation);

    ui->editSessionDuration->installEventFilter(this);

    connect(&delayTimer, SIGNAL(timeout()), this, SLOT(ForceMigration()));
}

/**
 * @brief SessionWindow::LoadSettings
 */
void SessionWindow::LoadSettings()
{
    QSettings settings;
    settings.beginGroup(QLatin1String("DTProgramSettings"));

    alternativeSaveLocation = settings.value(QLatin1String("alternateSaveLocation"), "").toString();

    if (!alternativeSaveLocation.isEmpty())
    {
        alternativeSaveLocationUnmod = QString(alternativeSaveLocation);
        alternativeSaveLocation = FileTools::pathAppend(alternativeSaveLocation, "DataTracker3");
    }

    showPlots = settings.value(QLatin1String("displayPlots"), false).toBool();
    outputSheets = settings.value(QLatin1String("outputSheets"), true).toBool();
    autoReli = settings.value(QLatin1String("autoReli"), false).toBool();
    autoMigrate = settings.value(QLatin1String("autoMigrate"), false).toBool();

    settings.endGroup();
}

/**
 * @brief SessionWindow::ForceMigration
 */
void SessionWindow::ForceMigration()
{
    if (!alternativeSaveLocation.isEmpty() && QDir(alternativeSaveLocation).exists())
    {
        migraterThread = new QThread();

        migrater = new FileMigrater(mWorkingDirectory, alternativeSaveLocationUnmod, "DataTracker3");
        migrater->moveToThread(migraterThread);

        connect(migrater, SIGNAL(workStarted()), migraterThread, SLOT(start()));
        connect(migraterThread, SIGNAL(started()), migrater, SLOT(working()));
        connect(migrater, SIGNAL(workFinished(QString)), migraterThread, SLOT(quit()), Qt::DirectConnection);
        connect(migrater, SIGNAL(workFinished(QString)), this, SLOT(ForceMigrationFinished(QString)));

        migraterThread->wait();
        migrater->startWork();
    }
}

void SessionWindow::ForceMigrationFinished(QString results)
{
    qDebug() << results;
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

            if (!mPresentDirectory.exists()){
                mPresentDirectory.mkdir(".");
            }

            PrepareDirectory();
        }
    }
}

/** A group is selected
 * @brief SessionWindow::on_comboGroup_currentIndexChanged
 * @param index
 */
void SessionWindow::on_comboGroup_currentIndexChanged(int index)
{
    PrepareGroup(index);
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

            PrepareGroup(ui->comboGroup->currentIndex());

            delayTimer.setSingleShot(true);
            delayTimer.start(2000);
        }
    }
}

/** An individual is selected
 * @brief SessionWindow::on_comboIndividual_currentIndexChanged
 * @param index
 */
void SessionWindow::on_comboIndividual_currentIndexChanged(int index)
{
    PrepareIndividual(index);
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

            PrepareIndividual(ui->comboIndividual->currentIndex());

            delayTimer.setSingleShot(true);
            delayTimer.start(2000);
        }
    }
}

/** An evaluation is selected
 * @brief SessionWindow::on_comboEvaluation_currentIndexChanged
 * @param index
 */
void SessionWindow::on_comboEvaluation_currentIndexChanged(int index)
{
    PrepareEvaluation(index);

    PrepareSessionCounter();
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

            PrepareEvaluation(ui->comboEvaluation->currentIndex());

            delayTimer.setSingleShot(true);
            delayTimer.start(2000);
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

        if (ok && (keySetName == "Collectors" || keySetName == "Therapists"))
        {
            QMessageBox::critical(NULL, tr("Name not allowed"),
                                  tr("The names 'Collectors' and 'Therapists' are not allowed. Please rename."),
                                  QMessageBox::Ok);

            return;
        }

        if (ok && !keySetName.isEmpty())
        {
            KeySetEditor mKeySetEntry;
            mKeySetEntry.KeySetName = keySetName;

            mKeySetEntry.exec();

            QString mKeyPath = FileTools::pathAppend(mWorkingDirectory, ui->comboGroup->currentText());
            mKeyPath = FileTools::pathAppend(mKeyPath, ui->comboIndividual->currentText());

            FileTools::WriteKeySet(FileTools::pathAppend(mKeyPath, QString("%1.json").arg(keySetName)), mKeySetEntry.keySet);

            if (!alternativeSaveLocation.isEmpty())
            {
                mKeyPath = FileTools::pathAppend(alternativeSaveLocation, ui->comboGroup->currentText());
                mKeyPath = FileTools::pathAppend(mKeyPath, ui->comboIndividual->currentText());

                FileTools::WriteKeySet(FileTools::pathAppend(mKeyPath, QString("%1.json").arg(keySetName)), mKeySetEntry.keySet);
            }

            delayTimer.setSingleShot(true);
            delayTimer.start(2000);
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

                    if (!alternativeSaveLocation.isEmpty())
                    {
                        mKeyPath = FileTools::pathAppend(alternativeSaveLocation, ui->comboGroup->currentText());
                        mKeyPath = FileTools::pathAppend(mKeyPath, ui->comboIndividual->currentText());

                        FileTools::WriteKeySet(FileTools::pathAppend(mKeyPath, QString("%1.json").arg(keySetName)), mKeySetEntry.keySet);
                    }

                    delayTimer.setSingleShot(true);
                    delayTimer.start(2000);
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

                if (!alternativeSaveLocation.isEmpty())
                {
                    QString mKeyPath = FileTools::pathAppend(alternativeSaveLocation, ui->comboGroup->currentText());
                    mKeyPath = FileTools::pathAppend(mKeyPath, ui->comboIndividual->currentText());

                    FileTools::WriteKeySet(FileTools::pathAppend(mKeyPath, QString("%1.json").arg(ui->comboKeySet->currentText())), mKeySetEntry.keySet);
                }

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

                delayTimer.setSingleShot(true);
                delayTimer.start(2000);
            }
        }
    }
}

/** Fire when keyboard changes
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

        if (!alternativeSaveLocation.isEmpty())
        {
            mTherapistPath = FileTools::pathAppend(alternativeSaveLocation, ui->comboGroup->currentText());
            mTherapistPath = FileTools::pathAppend(mTherapistPath, ui->comboIndividual->currentText());
            mTherapistPath = FileTools::pathAppend(mTherapistPath, "Therapists.json");

            FileTools::WriteTherapists(mTherapistPath, therapistList);
        }

        delayTimer.setSingleShot(true);
        delayTimer.start(2000);
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

        if (!alternativeSaveLocation.isEmpty())
        {
            mCollectorPath = FileTools::pathAppend(alternativeSaveLocation, ui->comboGroup->currentText());
            mCollectorPath = FileTools::pathAppend(mCollectorPath, ui->comboIndividual->currentText());
            mCollectorPath = FileTools::pathAppend(mCollectorPath, "Collectors.json");

            FileTools::WriteCollectors(mCollectorPath, collectorList);
        }

        delayTimer.setSingleShot(true);
        delayTimer.start(2000);
    }
}

/** Combo session update
 * @brief SessionWindow::on_comboSessionDuration_currentIndexChanged
 * @param index
 */
void SessionWindow::on_comboSessionDuration_currentIndexChanged(int index)
{
    if (index == ui->comboSessionDuration->count() - 1)
    {
        ui->editSessionDuration->setReadOnly(false);
    }
    else
    {
        ui->editSessionDuration->setReadOnly(true);
    }
}

/**
 * @brief SessionWindow::PrepareSessionCounter
 */
void SessionWindow::PrepareSessionCounter()
{
    counterThread = new QThread();

    counter = new SessionCounter(mWorkingDirectory,
                                 ui->comboGroup->currentText(),
                                 ui->comboIndividual->currentText(),
                                 ui->comboEvaluation->currentText());

    counter->moveToThread(counterThread);

    connect(counter, SIGNAL(workStarted()), counterThread, SLOT(start()));
    connect(counterThread, SIGNAL(started()), counter, SLOT(working()));
    connect(counter, SIGNAL(workFinished(int)), counterThread, SLOT(quit()), Qt::DirectConnection);
    connect(counter, SIGNAL(workingResult(QString)), this, SLOT(WorkUpdate(QString)));
    connect(counter, SIGNAL(workFinished(int)), this, SLOT(UpdateSessionCount(int)));

    counterThread->wait();
    counter->startWork();
}

/**
 * @brief SessionWindow::PrepareDirectory
 */
void SessionWindow::PrepareDirectory()
{
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

    delayTimer.setSingleShot(true);
    delayTimer.start(2000);
}

/**
 * @brief SessionWindow::PrepareGroup
 * @param index
 */
void SessionWindow::PrepareGroup(int index)
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

/**
 * @brief SessionWindow::PrepareIndividual
 * @param index
 */
void SessionWindow::PrepareIndividual(int index)
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

/**
 * @brief SessionWindow::PrepareEvaluation
 * @param index
 */
void SessionWindow::PrepareEvaluation(int index)
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
 * @brief SessionWindow::FileWriteFinished
 * @param result
 */
void SessionWindow::FileWriteFinished(QString result)
{
    if (mResults.isVisible())
    {
        mResults.SetTitle(result);
    }

    delayTimer.setSingleShot(true);
    delayTimer.start(2000);
}

/**
 * @brief SessionWindow::UpdateSessionCount
 * @param session
 */
void SessionWindow::UpdateSessionCount(int session)
{
    ui->editSessionNumber->setText(QString::number(session));
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

/**
 * @brief SessionWindow::EditCurrentKeySet
 */
void SessionWindow::EditCurrentKeySet()
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

    if (autoMigrate) { ForceMigration(); }
}

SessionWindow::~SessionWindow()
{
    delete ui;
}

/** Focus checks
 * @brief SessionWindow::eventFilter
 * @param obj
 * @param e
 * @return
 */
bool SessionWindow::eventFilter(QObject *obj, QEvent *e)
{
    if (e->type() == QEvent::FocusIn)
    {
        if (obj == ui->editSessionDuration && !sessionDurationDialog.AcceptDialog)
        {
            sessionDurationDialog.AcceptDialog = true;

            if (sessionDurationDialog.exec() == QDialog::Accepted)
            {
                ui->editSessionDuration->setText(QString("%1 Seconds").arg(sessionDurationDialog.GetSeconds()));
            }

            ui->editSessionDuration->clearFocus();

            ui->comboSessionDuration->setCurrentIndex(ui->comboSessionDuration->count() - 1);

            sessionDurationDialog.AcceptDialog = false;
        }
    }

    return false;
}

/** Get Duration Session
 * @brief SessionWindow::GetSessionDuration
 * @return
 */
int SessionWindow::GetSessionDuration()
{
    bool ok;
    QString mDuration;
    QStringList durationValue;

    if (ui->comboSessionDuration->currentIndex() == 0)
    {
        QMessageBox::critical(NULL, tr("Sussion Duration"),
                              tr("Set a duration for the session."),
                              QMessageBox::Ok);

        return -1;
    }
    else if (ui->comboSessionDuration->currentIndex() == ui->comboSessionDuration->count() - 1)
    {
        mDuration = ui->editSessionDuration->text();
        durationValue = mDuration.split(' ');
        durationValue[0].toInt(&ok);

        if (!ok)
        {
            QMessageBox::critical(NULL, tr("Custom Sussion Duration"),
                                  tr("The custom session duration isn't usable."),
                                  QMessageBox::Ok);

            return -1;
        }
        else
        {
            return durationValue[0].toInt(&ok);
        }
    }
    else
    {
        mDuration = ui->comboSessionDuration->currentText();
        durationValue = mDuration.split(' ');
        durationValue[0].toInt(&ok);

        if (ok)
        {
            return durationValue[0].toInt(&ok) * 60;
        }
    }

    return -1;
}

/** Get Session Number
 * @brief SessionWindow::GetSessionNumber
 * @return
 */
int SessionWindow::GetSessionNumber()
{
    bool ok;
    QString text = ui->editSessionNumber->text();

    text.toInt(&ok);

    if (ok)
    {
        return text.toInt();
    }

    return -1;
}

/**
 * @brief SessionWindow::ScoreAvailableKeys
 */
void SessionWindow::ScoreAvailableKeys()
{
    ScoringTools::ScoreOverallSchedule(&recordingWindow->PressedKeys, &CurrentKeySet,
                                       &recordingWindow->startTime, &recordingWindow->endTime,
                                       &mResults.FrequencyOverall, &mResults.DurationOverall,
                                       &mResults.TimeOverall);

    ScoringTools::ScoreSpecificSchedule(&recordingWindow->PressedKeys, &CurrentKeySet,
                                        &recordingWindow->endTime, Schedule::One,
                                        &mResults.FrequencyOne, &mResults.DurationOne,
                                        &mResults.TimeOne);

    ScoringTools::ScoreSpecificSchedule(&recordingWindow->PressedKeys, &CurrentKeySet,
                                        &recordingWindow->endTime, Schedule::Two,
                                        &mResults.FrequencyTwo, &mResults.DurationTwo,
                                        &mResults.TimeTwo);

    ScoringTools::ScoreSpecificSchedule(&recordingWindow->PressedKeys, &CurrentKeySet,
                                        &recordingWindow->endTime, Schedule::Three,
                                        &mResults.FrequencyThree, &mResults.DurationThree,
                                        &mResults.TimeThree);
}

/**
 * @brief SessionWindow::BuildResults
 */
void SessionWindow::BuildResults()
{
    mResults.SetKeySet(CurrentKeySet);
    mResults.BuildTables();
    mResults.BuildNarrative(&recordingWindow->PressedKeys, &recordingWindow->startTime);
    mResults.SetParameters(ui->comboGroup->currentText(),
                           ui->comboIndividual->currentText(),
                           ui->comboEvaluation->currentText(),
                           ui->comboCondition->currentText(),
                           ui->comboTherapist->currentText(),
                           ui->comboKeySet->currentText(),
                           ui->comboCollector->currentText(),
                           ui->comboRole->currentText());
}

/**
 * @brief SessionWindow::WriteOutput
 */
void SessionWindow::WriteOutput()
{
    fileWriteThread = new QThread();

    fileWriter = new FileWriter(mWorkingDirectory,
                                CurrentKeySet,
                                ui->comboGroup->currentText(),
                                ui->comboIndividual->currentText(),
                                ui->comboEvaluation->currentText(),
                                ui->comboCondition->currentText(),
                                ui->comboTherapist->currentText(),
                                ui->comboKeySet->currentText(),
                                ui->comboCollector->currentText(),
                                ui->comboRole->currentText(),
                                recordingWindow->startTime.toString(),
                                recordingWindow->endTime.toString(),
                                mResults.TimeOverall,
                                mResults.TimeOne,
                                mResults.TimeTwo,
                                mResults.TimeThree,
                                recordingWindow->PressedKeys,
                                &mResults.FrequencyOverall,
                                &mResults.DurationOverall,
                                &mResults.FrequencyOne,
                                &mResults.DurationOne,
                                &mResults.FrequencyTwo,
                                &mResults.DurationTwo,
                                &mResults.FrequencyThree,
                                &mResults.DurationThree,
                                outputSheets,
                                autoReli,
                                alternativeSaveLocation);

    fileWriter->moveToThread(fileWriteThread);

    connect(fileWriter, SIGNAL(workStarted()), fileWriteThread, SLOT(start()));
    connect(fileWriteThread, SIGNAL(started()), fileWriter, SLOT(working()));
    connect(fileWriter, SIGNAL(workFinished(QString)), fileWriteThread, SLOT(quit()), Qt::DirectConnection);
    connect(fileWriter, SIGNAL(workFinished(QString)), this, SLOT(FileWriteFinished(QString)));

    fileWriteThread->wait();
    fileWriter->startWork();

    if (showPlots)
    {
        mResults.SetTabEnabled(1, true);
        mResults.SetTabEnabled(2, true);

        mResults.BuildPlot(CurrentKeySet, &recordingWindow->PressedKeys, &recordingWindow->startTime, &recordingWindow->endTime);
    }
    else
    {
        mResults.SetTabEnabled(1, false);
        mResults.SetTabEnabled(2, false);
    }
}

/** Begin session
 * @brief SessionWindow::on_buttonBox_clicked
 * @param button
 */
void SessionWindow::on_buttonBox_clicked(QAbstractButton *button)
{
    if((QPushButton *)button == ui->buttonBox->button(QDialogButtonBox::Ok))
    {
        CurrentKeySet.TotalSeconds = GetSessionDuration();
        CurrentKeySet.Session = GetSessionNumber();

        if (CurrentKeySet.TotalSeconds == -1 || CurrentKeySet.Session == -1)
        {
            return;
        }

        recordingWindow = new RecordingWindow();
        recordingWindow->LoadKeys(CurrentKeySet);
        recordingWindow->SetGroup(ui->comboGroup->currentText());
        recordingWindow->SetIndividual(ui->comboIndividual->currentText());
        recordingWindow->SetEvaluation(ui->comboEvaluation->currentText());
        recordingWindow->SetCondition(ui->comboCondition->currentText());
        recordingWindow->SetCollector(ui->comboCollector->currentText());
        recordingWindow->SetRole(ui->comboRole->currentText());

        recordingWindow->setWindowState(recordingWindow->windowState() | Qt::WindowMaximized);

        if (recordingWindow->exec() == QDialog::Rejected)
        {
            return;
        }

        ScoreAvailableKeys();

        BuildResults();

        WriteOutput();

        mResults.exec();

        // Session counter

        counterThread = new QThread();

        counter = new SessionCounter(mWorkingDirectory,
                                     ui->comboGroup->currentText(),
                                     ui->comboIndividual->currentText(),
                                     ui->comboEvaluation->currentText());

        counter->moveToThread(counterThread);

        connect(counter, SIGNAL(workStarted()), counterThread, SLOT(start()));
        connect(counterThread, SIGNAL(started()), counter, SLOT(working()));
        connect(counter, SIGNAL(workFinished(int)), counterThread, SLOT(quit()), Qt::DirectConnection);
        connect(counter, SIGNAL(workingResult(QString)), this, SLOT(WorkUpdate(QString)));
        connect(counter, SIGNAL(workFinished(int)), this, SLOT(UpdateSessionCount(int)));

        counterThread->wait();
        counter->startWork();

        delayTimer.setSingleShot(true);
        delayTimer.start(2000);
    }
}

/**
 * @brief SessionWindow::on_tableFrequency_doubleClicked
 */
void SessionWindow::on_tableFrequency_doubleClicked(const QModelIndex)
{
    if (ui->comboGroup->currentIndex() == 0 ||
        ui->comboIndividual->currentIndex() == 0 ||
        ui->comboEvaluation->currentIndex() == 0 ||
        ui->comboKeySet->currentIndex() == 0)
    {
        return;
    }

    EditCurrentKeySet();
}

/**
 * @brief SessionWindow::on_tableDuration_doubleClicked
 */
void SessionWindow::on_tableDuration_doubleClicked(const QModelIndex)
{
    if (ui->comboGroup->currentIndex() == 0 ||
        ui->comboIndividual->currentIndex() == 0 ||
        ui->comboEvaluation->currentIndex() == 0 ||
        ui->comboKeySet->currentIndex() == 0)
    {
        return;
    }

    EditCurrentKeySet();
}
