#include "sessionwindow.h"
#include "ui_sessionwindow.h"

#include "filetools.h"
#include "parsetypes.h"

#include <QtWidgets>

SessionWindow::SessionWindow(QString mCurrentWorkingDirectory, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SessionWindow)
{
    ui->setupUi(this);

    mWorkingDirectory = mCurrentWorkingDirectory;

    setWindowTitle(tr("Session Designer"));

    /*
    connect(ui->comboGroup, SIGNAL(currentTextChanged(const QString&)), this, SLOT(UserChangedSelection(const QString&)));
    connect(ui->comboGroup, SIGNAL(currentTextChanged(const QString&)), this, SLOT(UserChangedSelection(const QString&)));
    connect(ui->comboGroup, SIGNAL(currentTextChanged(const QString&)), this, SLOT(UserChangedSelection(const QString&)));
    connect(ui->comboGroup, SIGNAL(currentTextChanged(const QString&)), this, SLOT(UserChangedSelection(const QString&)));
    connect(ui->comboGroup, SIGNAL(currentTextChanged(const QString&)), this, SLOT(UserChangedSelection(const QString&)));
    connect(ui->comboGroup, SIGNAL(currentTextChanged(const QString&)), this, SLOT(UserChangedSelection(const QString&)));
    connect(ui->comboGroup, SIGNAL(currentTextChanged(const QString&)), this, SLOT(UserChangedSelection(const QString&)));
    */

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
        while (ui->comboGroup->count() > 1)
        {
            ui->comboGroup->removeItem(1);
        }

        ui->comboGroup->addItems(finalResult.Groups);

        while (ui->comboIndividual->count() > 1)
        {
            ui->comboIndividual->removeItem(1);
        }

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
    }
    else if (action == ParseTypes::Evaluation)
    {
        while (ui->comboEvaluation->count() > 1)
        {
            ui->comboEvaluation->removeItem(1);
        }

        ui->comboEvaluation->addItems(finalResult.Evaluations);

        while (ui->comboCondition->count() > 1)
        {
            ui->comboCondition->removeItem(1);
        }

        ui->comboCondition->setCurrentIndex(0);
    }
    else if (action == ParseTypes::Condition)
    {

        qDebug() << "Condition:";

        qDebug() << finalResult.Conditions;

        while (ui->comboCondition->count() > 1)
        {
            ui->comboCondition->removeItem(1);
        }

        ui->comboCondition->addItems(finalResult.Conditions);
    }
}

SessionWindow::~SessionWindow()
{
    delete ui;
}






