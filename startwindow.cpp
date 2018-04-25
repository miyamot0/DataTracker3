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

#include "startwindow.h"
#include "ui_startwindow.h"

#include "sessionwindow.h"
#include "windowtools.h"
#include "filetools.h"

#include <QStandardPaths>
#include <QMessageBox>
#include <QTextStream>
#include <QDir>

StartWindow::StartWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::StartWindow)
{
    ui->setupUi(this);

    QSettings settings(QSettings::UserScope, QLatin1String("Data Tracker"));

    folderTitle = "DataTracker3";
    setWindowTitle(QString(tr("Data Tracker")) + " v" + QString("%1.%2.%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUILD));

    if (FileTools::CheckAndPrepDirectory(folderTitle))
    {
        workingDirectory = FileTools::pathAppend(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0], folderTitle);
    }

    LoadSettings();

    if (QDir(backupSaveLocation).exists())
    {
        /*
         *
         * Disable for now
         *
         *
        migraterThread = new QThread();
        migrater = new FileMigrater(workingDirectory, backupSaveLocation, folderTitle);
        migrater->moveToThread(migraterThread);

        connect(migrater, SIGNAL(workStarted()), migraterThread, SLOT(start()));
        connect(migraterThread, SIGNAL(started()), migrater, SLOT(working()));
        connect(migrater, SIGNAL(workFinished(QString)), migraterThread, SLOT(quit()), Qt::DirectConnection);
        connect(migrater, SIGNAL(workFinished(QString)), this, SLOT(WorkFinished(QString)));

        migraterThread->wait();
        migrater->startWork();
        */
    }

    statusBar()->setSizeGripEnabled(false);

    WindowTools::SetWindowFixed(this);

    if (autoUpdateCheck)
    {
        manager = new QNetworkAccessManager(this);
        connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(downloadedFile(QNetworkReply*)) );

        #ifdef _WIN32
            manager->get(QNetworkRequest(QUrl("http://www.smallnstats.com/DataTrackerRepository/Updates.xml")));
        #elif TARGET_OS_MAC
            manager->get(QNetworkRequest(QUrl("http://www.smallnstats.com/DataTrackerRepositoryMac/Updates.xml")));
        #endif
    }

    QPixmap source(":/images/BackgroundScaled.png");

    QImage image(this->size(), QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::white);

    QPainter painter(&image);
    painter.setOpacity(0.2);
    painter.drawPixmap(0, -10, source);
    painter.end();

    QPixmap background = QPixmap::fromImage(image);

    QPalette palette;
    palette.setBrush(QPalette::Background, background);
    this->setPalette(palette);
}

/** Downloaded file event
 * @brief SheetWidget::downloadedFile
 * @param reply
 */
void StartWindow::downloadedFile(QNetworkReply *reply) {
    QByteArray data = reply->readAll();

    QDomDocument versionXML;

    if(!versionXML.setContent(data))
    {
        return;
    }

    QDomElement root = versionXML.documentElement();
    QDomElement mNode = root.namedItem("PackageUpdate").toElement();
    QDomElement mNode2 = mNode.namedItem("Version").toElement();

    QStringList mVersionList = mNode2.text().split('.');

    if (mVersionList.count() != 3)
    {
        return;
    }

    bool hasUpdate = false;

    QString mNetworkVersionString = QString("%1%2%3").arg(mVersionList[0]).arg(mVersionList[1]).arg(mVersionList[2]);

    QString mLocalVersionString = QString("%1%2%3").arg(VERSION_MAJOR).arg(VERSION_MINOR).arg(VERSION_BUILD);

    if (mNetworkVersionString.toInt() > mLocalVersionString.toInt())
    {
        hasUpdate = true;
    }

    if (hasUpdate)
    {
        QMessageBox *msgBox = new QMessageBox;
        msgBox->setWindowTitle(tr("Updates"));
        msgBox->setText(tr("There is an update available!"));
        msgBox->setWindowModality(Qt::NonModal);
        msgBox->show();
    }
}

/** Launch updater process
 * @brief StartWindow::on_actionLaunch_Updater_triggered
 */
void StartWindow::on_actionLaunch_Updater_triggered()
{
    QString mCommand = "";

#ifdef _WIN32
    mCommand = "maintenancetool.exe";
#elif TARGET_OS_MAC
    QDir mDir = QDir(QCoreApplication::applicationDirPath());
    mDir.cdUp();
    mDir.cdUp();
    mDir.cdUp();

    mCommand = QDir::cleanPath(mDir.path() + QDir::separator() + "maintenancetool.app");
#endif

    if (QFile::exists(mCommand))
    {
        QProcess p;
        QStringList args;
        args << "--updater";
        p.start(mCommand, args);
        p.waitForStarted();
        p.waitForFinished(-1);
    }
}

/**
 * @brief StartWindow::WorkFinished
 * @param value
 */
void StartWindow::WorkFinished(QString value)
{
    statusBar()->showMessage(value, 3000);
}

/**
 * @brief StartWindow::~StartWindow
 */
StartWindow::~StartWindow()
{
    delete ui;
}

/** Save Settings
 * @brief StartWindow::SaveSettings
 */
void StartWindow::SaveSettings(QString savedLocation, bool plots, bool dark, bool sheets, bool reli, bool migrate, bool updateCheck)
{
    QSettings settings;

    settings.beginGroup(QLatin1String("DTProgramSettings"));
        settings.setValue(QLatin1String("alternateSaveLocation"), savedLocation);
        settings.setValue(QLatin1String("displayPlots"), plots);
        settings.setValue(QLatin1String("displayDark"), dark);
        settings.setValue(QLatin1String("outputSheets"), sheets);
        settings.setValue(QLatin1String("autoReli"), reli);
        settings.setValue(QLatin1String("autoMigrate"), migrate);
        settings.setValue(QLatin1String("autoUpdateCheck"), updateCheck);
    settings.endGroup();
}

/**
 * @brief StartWindow::closeEvent
 * @param event
 */
void StartWindow::closeEvent(QCloseEvent *)
{
    SaveSettings(backupSaveLocation,
                 displayPlots,
                 displayDark,
                 outputSheets,
                 autoReli,
                 autoMigrate,
                 autoUpdateCheck);
}

/** Load Settings
 * @brief StartWindow::LoadSettings
 */
void StartWindow::LoadSettings()
{
    QSettings settings;

    settings.beginGroup(QLatin1String("DTProgramSettings"));
    backupSaveLocation = settings.value(QLatin1String("alternateSaveLocation"), "").toString();
    displayPlots = settings.value(QLatin1String("displayPlots"), false).toBool();
    displayDark = settings.value(QLatin1String("displayDark"), false).toBool();
    outputSheets = settings.value(QLatin1String("outputSheets"), true).toBool();
    autoReli = settings.value(QLatin1String("autoReli"), true).toBool();
    autoMigrate = settings.value(QLatin1String("autoMigrate"), false).toBool();
    autoUpdateCheck = settings.value(QLatin1String("autoUpdateCheck"), true).toBool();

    settings.endGroup();
}

/** Close Window
 * @brief StartWindow::on_actionExit_triggered
 */
void StartWindow::on_actionExit_triggered()
{
    close();
}

/** Open Reli Window
 * @brief StartWindow::on_actionCalculate_Reliability_triggered
 */
void StartWindow::on_actionCalculate_Reliability_triggered()
{
    reliabilityDialog = new ReliabilityDialog(workingDirectory, this);
    reliabilityDialog->exec();
}

/** Open Settings Window
 * @brief StartWindow::on_actionSettings_2_triggered
 */
void StartWindow::on_actionSettings_2_triggered()
{
    settingsDialog.SetSaveLocation(backupSaveLocation);
    settingsDialog.SetSpreadsheetOption(outputSheets);
    settingsDialog.SetDisplayOption(displayPlots);
    settingsDialog.SetThemeDark(displayDark);
    settingsDialog.SetAutoReli(autoReli);
    settingsDialog.SetAutoMigrate(autoMigrate);
    settingsDialog.SetAutoUpdateCheck(autoUpdateCheck);

    settingsDialog.exec();

    if (displayDark != settingsDialog.GetThemeDark() || autoUpdateCheck != settingsDialog.GetAutoUpdate())
    {
        QMessageBox::information(NULL,
                                 tr("Settings Updated"),
                                 tr("Restart for changes to take effect."),
                                 QMessageBox::Ok);
    }

    backupSaveLocation = settingsDialog.GetSaveLocation();
    outputSheets = settingsDialog.GetSpreadsheetOption();
    displayPlots = settingsDialog.GetDisplayOption();
    displayDark = settingsDialog.GetThemeDark();
    autoReli = settingsDialog.GetAutoReli();
    autoMigrate = settingsDialog.GetAutoMigrate();
    autoUpdateCheck = settingsDialog.GetAutoUpdate();

    SaveSettings(backupSaveLocation,
                 displayPlots,
                 displayDark,
                 outputSheets,
                 autoReli,
                 autoMigrate,
                 autoUpdateCheck);
}

/** Open License Window
 * @brief StartWindow::on_actionQt_Framework_triggered
 */
void StartWindow::on_actionQt_Framework_triggered()
{
    QString mFilePath = "";

    #ifdef _WIN32
            mFilePath = "License_Qt.txt";
    #elif TARGET_OS_MAC
            QDir runDirectory = QDir(QCoreApplication::applicationDirPath());
            runDirectory.cdUp();
            runDirectory.cd("Resources");
            mFilePath = runDirectory.path() + "/";

            mFilePath = mFilePath + "License_Qt.txt";

    #endif

    licenseDialog = new LicenseDialog(mFilePath, this);
    licenseDialog->setWindowTitle(tr("Qt License (LGPL-V3)"));
    licenseDialog->setModal(true);
    licenseDialog->exec();
}

/** Open License Window
 * @brief StartWindow::on_actionQtXlsx_triggered
 */
void StartWindow::on_actionQtXlsx_triggered()
{
    QString mFilePath = "";

    #ifdef _WIN32
            mFilePath = "License_QtXlsx.txt";
    #elif TARGET_OS_MAC
            QDir runDirectory = QDir(QCoreApplication::applicationDirPath());
            runDirectory.cdUp();
            runDirectory.cd("Resources");
            mFilePath = runDirectory.path() + "/";

            mFilePath = mFilePath + "License_QtXlsx.txt";

    #endif

    licenseDialog = new LicenseDialog(mFilePath, this);
    licenseDialog->setWindowTitle(tr("QtXlsx License (MIT)"));
    licenseDialog->setModal(true);
    licenseDialog->exec();
}

/** Open License Window
 * @brief StartWindow::on_actionTango_Icons_triggered
 */
void StartWindow::on_actionTango_Icons_triggered()
{
    QString mFilePath = "";

    #ifdef _WIN32
            mFilePath = "License_Tango.txt";
    #elif TARGET_OS_MAC
            QDir runDirectory = QDir(QCoreApplication::applicationDirPath());
            runDirectory.cdUp();
            runDirectory.cd("Resources");
            mFilePath = runDirectory.path() + "/";

            mFilePath = mFilePath + "License_Tango.txt";

    #endif

    licenseDialog = new LicenseDialog(mFilePath, this);
    licenseDialog->setWindowTitle(tr("Tango Icon Set License (Public Domain)"));
    licenseDialog->setModal(true);
    licenseDialog->exec();
}

/** Open License Window
 * @brief StartWindow::on_actionQDarkStyleSheet_triggered
 */
void StartWindow::on_actionQDarkStyleSheet_triggered()
{
    QString mFilePath = "";

    #ifdef _WIN32
            mFilePath = "License_QDarkStyleSheet.txt";
    #elif TARGET_OS_MAC
            QDir runDirectory = QDir(QCoreApplication::applicationDirPath());
            runDirectory.cdUp();
            runDirectory.cd("Resources");
            mFilePath = runDirectory.path() + "/";

            mFilePath = mFilePath + "License_Tango.txt";

    #endif

    licenseDialog = new LicenseDialog(mFilePath, this);
    licenseDialog->setWindowTitle(tr("QDarkStyleSheet License (MIT)"));
    licenseDialog->setModal(true);
    licenseDialog->exec();
}

/** Open License Window
 * @brief StartWindow::on_actionBDataPro_triggered
 */
void StartWindow::on_actionBDataPro_triggered()
{
    QString mFilePath = "";

    #ifdef _WIN32
            mFilePath = "License_BDataPro.txt";
    #elif TARGET_OS_MAC
            QDir runDirectory = QDir(QCoreApplication::applicationDirPath());
            runDirectory.cdUp();
            runDirectory.cd("Resources");
            mFilePath = runDirectory.path() + "/";

            mFilePath = mFilePath + "License_BDataPro.txt";

    #endif

    licenseDialog = new LicenseDialog(mFilePath, this);
    licenseDialog->setWindowTitle(tr("BDataPro License (GPL-V3)"));
    licenseDialog->setModal(true);
    licenseDialog->exec();
}

/** Open Visual Analysis Window
 * @brief StartWindow::on_actionSession_Visual_Analysis_triggered
 */
void StartWindow::on_actionSession_Visual_Analysis_triggered()
{
    sessionViewerDialog = new SessionViewerDialog(workingDirectory, this);
    sessionViewerDialog->exec();
}

/**
 * @brief StartWindow::on_actionSequential_Analyses_triggered
 */
void StartWindow::on_actionSequential_Analyses_triggered()
{
    sequenceDialog = new SequentialAnalysisDialog(workingDirectory, this);
    sequenceDialog->exec();
}

void StartWindow::on_actionCondition_Sequential_Analysis_triggered()
{
    sequenceConditionDialog = new SequentialConditionAnalysisDialog(workingDirectory, this);
    sequenceConditionDialog->exec();
}

/** Open Session Viewer Dialog
 * @brief StartWindow::on_actionEvaluation_Visual_Analysis_triggered
 */
void StartWindow::on_actionEvaluation_Visual_Analysis_triggered()
{
    evaluationViewerDialog = new EvaluationViewerDialog(workingDirectory, this);
    evaluationViewerDialog->exec();
}

/** Open Evaluation Report Dialog
 * @brief StartWindow::on_actionEvaluation_Report_triggered
 */
void StartWindow::on_actionEvaluation_Report_triggered()
{
    evaluationReportDialog = new EvaluationReportDialog(workingDirectory, this);
    evaluationReportDialog->exec();
}

/** Open FAQ Window
 * @brief StartWindow::on_actionFrequently_Asked_Questions_triggered
 */
void StartWindow::on_actionFrequently_Asked_Questions_triggered()
{
    faqDialog.exec();
}

/** Open Contact Window
 * @brief StartWindow::on_actionContact_triggered
 */
void StartWindow::on_actionContact_triggered()
{
    contactDialog.exec();
}

/** Open About Window
 * @brief StartWindow::on_actionAbout_triggered
 */
void StartWindow::on_actionAbout_triggered()
{
    aboutDialogWindow.exec();
}

/** Open Session Window
 * @brief StartWindow::on_buttonStart_clicked
 */
void StartWindow::on_buttonStart_clicked()
{
    if (FileTools::CheckAndPrepDirectory(folderTitle))
    {
        sessionWindow = new SessionWindow(workingDirectory, this);
        sessionWindow->exec();
    }
}
