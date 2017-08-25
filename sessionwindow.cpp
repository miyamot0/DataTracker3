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
    QString text = QInputDialog::getText(this, tr("Enter New Group"), tr("Group Name:"), QLineEdit::Normal, QDir::home().dirName(), &ok);

    if (ok)
    {
        if (ui->comboGroup->findText(text) == -1)
        {
            QString mPath = FileTools::pathAppend(mWorkingDirectory, text);
            QDir mPresentDirectory(mPath);

            if (!mPresentDirectory.exists()){
                mPresentDirectory.mkdir(".");
            }

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
    }
}

/** A group is selected
 * @brief SessionWindow::on_comboGroup_currentIndexChanged
 * @param index
 */
void SessionWindow::on_comboGroup_currentIndexChanged(int index)
{
    QString mPath = FileTools::pathAppend(mWorkingDirectory, ui->comboGroup->currentText());

    QDir mPresentDirectory(mPath);

    if (!mPresentDirectory.exists()){
        mPresentDirectory.mkdir(".");
    }

    workerThread = new QThread();

    mCurrentDirectory.WorkingDirectory = mWorkingDirectory;
    mCurrentDirectory.CurrentGroup = ui->comboGroup->currentText();

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
    QString text = QInputDialog::getText(this, tr("Enter New Individual"), tr("Individual Name:"), QLineEdit::Normal, QDir::home().dirName(), &ok);

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

void SessionWindow::on_comboIndividual_currentIndexChanged(int index)
{

}

//thread
//get x, return x.y

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
        for (int g = ui->comboGroup->count() - 1; g > 0; g--)
        {
            ui->comboGroup->removeItem(g);
        }

        ui->comboGroup->addItems(finalResult.Groups);
    }
    else if (action == ParseTypes::Individual)
    {
        for (int g = ui->comboIndividual->count() - 1; g > 0; g--)
        {
            ui->comboIndividual->removeItem(g);
        }

        ui->comboIndividual->addItems(finalResult.Individuals);
    }
}

SessionWindow::~SessionWindow()
{
    delete ui;
}
