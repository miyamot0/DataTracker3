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

#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDomDocument>
#include <QThread>

#include "sessionwindow.h"
#include "filemigrater.h"

#include "reliabilitydialog.h"
#include "settingsdialog.h"
#include "faqdialog.h"
#include "aboutdialog.h"
#include "contactdialog.h"
#include "licensedialog.h"
#include "sessionviewerdialog.h"
#include "evaluationviewerdialog.h"
#include "evaluationreportdialog.h"

namespace Ui {
class StartWindow;
}

class StartWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = 0);
    ~StartWindow();

    void SaveSettings(QString savedLocation, bool plots, bool dark, bool sheets, bool reli, bool migrate);
    void LoadSettings();

private slots:
    void on_actionExit_triggered();
    void on_actionCalculate_Reliability_triggered();
    void on_actionSettings_2_triggered();
    void on_actionQt_Framework_triggered();
    void on_actionQtXlsx_triggered();
    void on_actionTango_Icons_triggered();
    void on_actionQDarkStyleSheet_triggered();
    void on_actionBDataPro_triggered();
    void on_actionSession_Visual_Analysis_triggered();
    void on_actionFrequently_Asked_Questions_triggered();
    void on_actionEvaluation_Visual_Analysis_triggered();
    void on_actionEvaluation_Report_triggered();
    void on_actionContact_triggered();
    void on_actionAbout_triggered();
    void on_buttonStart_clicked();

    void WorkFinished(QString value);

    void downloadedFile(QNetworkReply *reply);
    void on_actionLaunch_Updater_triggered();

private:
    Ui::StartWindow *ui;

    AboutDialog aboutDialogWindow;
    ContactDialog contactDialog;
    LicenseDialog *licenseDialog;
    SettingsDialog settingsDialog;
    SessionViewerDialog * sessionViewerDialog;
    EvaluationViewerDialog * evaluationViewerDialog;
    EvaluationReportDialog * evaluationReportDialog;
    FAQDialog faqDialog;

    ReliabilityDialog * reliabilityDialog;

    SessionWindow * sessionWindow;

    QString folderTitle;
    bool foundFolder = false;

    QString workingDirectory;
    QString backupSaveLocation;
    bool displayPlots;
    bool displayDark;
    bool outputSheets;
    bool autoReli;
    bool autoMigrate;

    QThread *migraterThread;
    FileMigrater *migrater;

    QNetworkAccessManager *manager;

    void closeEvent(QCloseEvent*);
};

#endif // STARTWINDOW_H
