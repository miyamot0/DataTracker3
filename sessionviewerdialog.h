#ifndef SESSIONVIEWERDIALOG_H
#define SESSIONVIEWERDIALOG_H

#include <QDialog>
#include <QJsonObject>
#include <QtCharts>

#include "directoryparse.h"
#include "directorysearcher.h"
#include "reliabilityparse.h"
#include "keysetentry.h"
#include "seriesselectdialog.h"
#include "sessionevent.h"

namespace Ui {
class SessionViewerDialog;
}

class SessionViewerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SessionViewerDialog(QString mCurrentWorkingDirectory, QWidget *parent = 0);
    ~SessionViewerDialog();

public slots:
    void WorkUpdate(QString update);
    void WorkFinished(DirectoryParse finalResult, ParseTypes::ParseAction action);

    void DrawBlankPlot();
    void DrawFrequencyPlot();
    void DrawDurationPlot();

private slots:
    void on_comboBoxGroup_currentIndexChanged(int index);
    void on_comboBoxIndividual_currentIndexChanged(int index);
    void on_comboBoxEvaluation_currentIndexChanged(int index);

    void on_tableWidget_currentCellChanged(int currentRow, int, int, int);
    void on_comboBoxDimension_currentIndexChanged(int index);
    void on_pushButton_clicked();

    void DrawFrequencySeries(int index);
    void DrawDurationSeries(int index);

private:
    Ui::SessionViewerDialog *ui;

    SeriesSelectDialog mSeriesSelect;

    DirectoryParse mCurrentDirectory;
    QString mWorkingDirectory;
    QThread *workerThread;
    DirectorySearcher *worker;

    QList<ReliabilityParse> PrimaryReliabilityObjects;

    QJsonObject json;

    QChart * chart;

    QChartView * chartView;

    QValueAxis * axisX,
               * axisY;

    QList<QLineSeries *> lineSeries;

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

    int max = 0,
        secs,
        fIndex,
        dIndex;

    QDateTime prev, after;

    bool waitingForNext;

    double runningSum,
           startSecs,
           endSecs;

    int tempKeyCode;

    QList<QString> fKeyDesc, dKeyDesc;

    QList<bool> fKeyShow, dKeyShow;

    void GetFrequencyKeySets();
    void GetDurationKeySets();

    QStringList fKeySeriesNames, dKeySeriesNames;

    QString formatReli(bool value)
    {
        return (value) ? "Reliability" : "Primary";
    }
};

#endif // SESSIONVIEWERDIALOG_H
