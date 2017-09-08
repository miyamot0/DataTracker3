#ifndef SEQUENTIALCONDITIONANALYSISDIALOG_H
#define SEQUENTIALCONDITIONANALYSISDIALOG_H

#include <QDialog>
#include <QThread>
#include <QDebug>
#include <QJsonObject>
#include <QJsonArray>

#include "directoryparse.h"
#include "directorysearcher.h"
#include "reliabilityparse.h"
#include "sessionevent.h"
#include "seriesselectdialog.h"

namespace Ui {
class SequentialConditionAnalysisDialog;
}

class SequentialConditionAnalysisDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SequentialConditionAnalysisDialog(QString mCurrentWorkingDirectory, QWidget *parent = 0);
    ~SequentialConditionAnalysisDialog();

private slots:

    void WorkUpdate(QString update);
    void WorkFinished(DirectoryParse finalResult, ParseTypes::ParseAction action);

    void on_comboBoxGroup_currentIndexChanged(int index);
    void on_comboBoxIndividual_currentIndexChanged(int index);
    void on_comboBoxEvaluation_currentIndexChanged(int index);
    void on_comboBoxCondition_currentIndexChanged(int index);
    void on_comboBoxWindowSize_currentIndexChanged(int index);

    void on_pushButton_clicked();

    void GetKeys();
    void ChartYule();
    int GetWindowSpan();
    QColor GetColorMapping(QString value);

private:
    Ui::SequentialConditionAnalysisDialog *ui;

    SeriesSelectDialog mSeriesSelect;

    DirectoryParse mCurrentDirectory;
    QString mWorkingDirectory;
    QThread * workerThread;
    DirectorySearcher * worker;

    QList<ReliabilityParse> PrimaryReliabilityObjects;
    QList<ReliabilityParse> SecondaryReliabilityObjects;

    QList<QList<QStringList>> ReportResults;

    QJsonObject json;

    ReliabilityParse temp;
    bool result;

    QList<SessionEvent> PressedKeys;
    QJsonArray pressedKeysJson;

    QStringList keyList;
    QList<bool> keyShowList;
    QList<QPair<QString, int>> mScoreKey;
};

#endif // SEQUENTIALCONDITIONANALYSISDIALOG_H
