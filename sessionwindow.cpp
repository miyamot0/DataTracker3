#include "sessionwindow.h"
#include "ui_sessionwindow.h"

#include "filetools.h"
#include "parsetypes.h"
#include "keyseteditor.h"

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

void SessionWindow::on_buttonKeySet_clicked()
{
    if (ui->comboGroup->currentIndex() == 0
            || ui->comboIndividual->currentIndex() == 0)
    {
        return;
    }

    QStringList listOptions;
    listOptions << tr("Add KeySet") << tr("Edit KeySet");

    bool ok;
    QString selectedOption = QInputDialog::getItem(this, tr("QInputDialog::getItem()"),
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
            // TODO modify keys

        }
    }
}

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

void SessionWindow::UserChangedSelection(const QString&)
{

}

void SessionWindow::WorkUpdate(QString update)
{
    qDebug() << "WORK_UPDATE: " << update;
}

void SessionWindow::WorkFinished(DirectoryParse finalResult, ParseTypes::ParseAction action)
{
    if (action == ParseTypes::Group)
    {
        DefaultComboBox(ui->comboGroup);
        //while (ui->comboGroup->count() > 1)
        //{
        //    ui->comboGroup->removeItem(1);
        //}

        ui->comboGroup->addItems(finalResult.Groups);

        DefaultComboBox(ui->comboIndividual);
        //while (ui->comboIndividual->count() > 1)
        //{
        //    ui->comboIndividual->removeItem(1);
        //}

        ui->comboIndividual->setCurrentIndex(0);

        while (ui->comboEvaluation->count() > 1)
        {
            ui->comboEvaluation->removeItem(1);
        }

        ui->comboEvaluation->setCurrentIndex(0);

        while (ui->comboCondition->count() > 1)
        {
            ui->comboCondition->removeItem(1);
        }

        ui->comboCondition->setCurrentIndex(0);

        while (ui->comboKeySet->count() > 1)
        {
            ui->comboKeySet->removeItem(1);
        }

        ui->comboKeySet->setCurrentIndex(0);

        while (ui->comboTherapist->count() > 1)
        {
            ui->comboTherapist->removeItem(1);
        }

        ui->comboTherapist->setCurrentIndex(0);
    }
    else if (action == ParseTypes::Individual)
    {
        while (ui->comboIndividual->count() > 1)
        {
            ui->comboIndividual->removeItem(1);
        }

        ui->comboIndividual->addItems(finalResult.Individuals);

        while (ui->comboEvaluation->count() > 1)
        {
            ui->comboEvaluation->removeItem(1);
        }

        ui->comboEvaluation->setCurrentIndex(0);

        while (ui->comboCondition->count() > 1)
        {
            ui->comboCondition->removeItem(1);
        }

        ui->comboCondition->setCurrentIndex(0);

        while (ui->comboKeySet->count() > 1)
        {
            ui->comboKeySet->removeItem(1);
        }

        ui->comboKeySet->setCurrentIndex(0);

        while (ui->comboTherapist->count() > 1)
        {
            ui->comboTherapist->removeItem(1);
        }

        ui->comboTherapist->setCurrentIndex(0);
    }
    else if (action == ParseTypes::Evaluation)
    {
        while (ui->comboEvaluation->count() > 1)
        {
            ui->comboEvaluation->removeItem(1);
        }

        ui->comboEvaluation->addItems(finalResult.Evaluations);

        while (ui->comboKeySet->count() > 1)
        {
            ui->comboKeySet->removeItem(1);
        }

        ui->comboKeySet->addItems(finalResult.KeySets);

        // TODO therapists here

        while (ui->comboCondition->count() > 1)
        {
            ui->comboCondition->removeItem(1);
        }

        ui->comboCondition->setCurrentIndex(0);
    }
    else if (action == ParseTypes::Condition)
    {
        while (ui->comboCondition->count() > 1)
        {
            ui->comboCondition->removeItem(1);
        }

        ui->comboCondition->addItems(finalResult.Conditions);
    }
}

void SessionWindow::DefaultComboBox(QComboBox *mSelectedBox)
{
    while (mSelectedBox->count() > 1)
    {
        mSelectedBox->removeItem(1);
    }
}

SessionWindow::~SessionWindow()
{
    delete ui;
}









