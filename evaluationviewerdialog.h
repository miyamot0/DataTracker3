#ifndef EVALUATIONVIEWERDIALOG_H
#define EVALUATIONVIEWERDIALOG_H

#include <QDialog>
#include <QtCharts>

#include "directoryparse.h"
#include "directorysearcher.h"

#include "reliabilityparse.h"


namespace Ui {
class EvaluationViewerDialog;
}

class EvaluationViewerDialog : public QDialog
{
    Q_OBJECT

public:
    explicit EvaluationViewerDialog(QString mCurrentWorkingDirectory, QWidget *parent = 0);
    ~EvaluationViewerDialog();

public slots:
    void WorkUpdate(QString update);
    void WorkFinished(DirectoryParse finalResult, ParseTypes::ParseAction action);
    void DrawPlot();

private slots:
    void on_comboBoxGroup_currentIndexChanged(int index);
    void on_comboBoxIndividual_currentIndexChanged(int index);
    void on_comboBoxEvaluation_currentIndexChanged(int index);

    void on_pushButton_clicked();

private:
    Ui::EvaluationViewerDialog *ui;

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
};

#endif // EVALUATIONVIEWERDIALOG_H
