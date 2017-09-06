#ifndef SEQUENTIALANALYSISDIALOG_H
#define SEQUENTIALANALYSISDIALOG_H

#include <QDialog>
#include <QJsonArray>
#include <QJsonObject>
#include <QThread>
#include <QDebug>

#include "directoryparse.h"
#include "parsetypes.h"
#include "seriesselectdialog.h"
#include "directorysearcher.h"
#include "reliabilityparse.h"

#include "keysetentry.h"
#include "sessionevent.h"

namespace Ui {
class SequentialAnalysisDialog;
}

class SequentialAnalysisDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SequentialAnalysisDialog(QString mCurrentWorkingDirectory, QWidget *parent = 0);
    ~SequentialAnalysisDialog();

private slots:
    void on_comboBoxGroup_currentIndexChanged(int index);
    void on_comboBoxIndividual_currentIndexChanged(int index);
    void on_comboBoxEvaluation_currentIndexChanged(int index);
    void on_comboBoxAnalysis_currentIndexChanged(int index);
    void on_comboBoxWindow_currentIndexChanged(int index);

    void WorkUpdate(QString update);
    void WorkFinished(DirectoryParse finalResult, ParseTypes::ParseAction action);

    void GetKeys();
    void CalculateStats();

    void on_tableWidget_currentCellChanged(int rowChanged, int, int, int);

    void ChartOddsRatio(int index);
    void ChartYule(int index);


private:
    Ui::SequentialAnalysisDialog *ui;

    SeriesSelectDialog mSeriesSelect;

    DirectoryParse mCurrentDirectory;
    QString mWorkingDirectory;
    QThread *workerThread;
    DirectorySearcher *worker;

    QList<ReliabilityParse> PrimaryReliabilityObjects;

    QJsonObject json;

    QList<int> fKeySet;
    QList<int> fKeySum;

    QList<int> dKeySet;
    QList<double> dKeySum;

    ReliabilityParse temp;
    bool result;

    QDateTime startTime, endTime;

    int totalSecs;

    QList<KeySetEntry> FrequencyKeys;
    QJsonArray frequencyArray;

    QList<KeySetEntry> DurationKeys;
    QJsonArray durationArray;

    QList<SessionEvent> PressedKeys;
    QJsonArray pressedKeysJson;

    QStringList keyList;
    QList<bool> keyShowList;
};

#endif // SEQUENTIALANALYSISDIALOG_H
