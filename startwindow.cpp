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

    // TODO network auto update
}

StartWindow::~StartWindow()
{
    delete ui;
}

/** Save Settings
 * @brief StartWindow::SaveSettings
 */
void StartWindow::SaveSettings(QString savedLocation, bool plots, bool dark, bool sheets)
{
    QSettings settings;

    settings.beginGroup(QLatin1String("DTProgramSettings"));
        settings.setValue(QLatin1String("alternateSaveLocation"), savedLocation);
        settings.setValue(QLatin1String("displayPlots"), plots);
        settings.setValue(QLatin1String("displayDark"), dark);
        settings.setValue(QLatin1String("outputSheets"), sheets);
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
                 outputSheets);
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
    reliabilityDialog.exec();
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

    settingsDialog.exec();

    if (displayDark != settingsDialog.displayDark)
    {
        QMessageBox::information(NULL,
                                 tr("Theme Updated"),
                                 tr("Restart for theme changes to take effect."),
                                 QMessageBox::Ok);
    }

    backupSaveLocation = settingsDialog.GetSaveLocation();
    outputSheets = settingsDialog.GetSpreadsheetOption();
    displayPlots = settingsDialog.GetDisplayOption();
    displayDark = settingsDialog.GetThemeDark();

    SaveSettings(backupSaveLocation,
                 displayPlots,
                 displayDark,
                 outputSheets);
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
