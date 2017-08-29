#ifndef SESSIONVIEWERDIALOG_H
#define SESSIONVIEWERDIALOG_H

#include <QDialog>
#include <QJsonObject>
#include <QtCharts>

#include "directoryparse.h"
#include "directorysearcher.h"

#include "reliabilityparse.h"

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

private slots:
    void on_comboBoxGroup_currentIndexChanged(int index);
    void on_comboBoxIndividual_currentIndexChanged(int index);
    void on_comboBoxEvaluation_currentIndexChanged(int index);

    void on_tableWidget_currentCellChanged(int currentRow, int, int, int);

private:
    Ui::SessionViewerDialog *ui;

    DirectoryParse mCurrentDirectory;
    QString mWorkingDirectory;
    QThread *workerThread;
    DirectorySearcher *worker;

    QList<ReliabilityParse> PrimaryReliabilityObjects;

    QJsonObject json;

    QChart * chart;
    QChart * chart2;

    QChartView * chartView;
    QChartView * chartView2;

    QValueAxis * axisX,
               * axisY,
               * axisX2,
               * axisY2;

    QList<QLineSeries *> lineSeries,
                         lineSeries2;

    QList<int> fKeySet;
    QList<int> fKeySum;

    QList<int> dKeySet;
    QList<double> dKeySum;

    QString formatReli(bool value)
    {
        return (value) ? "Reliability" : "Primary";
    }
};

#endif // SESSIONVIEWERDIALOG_H
