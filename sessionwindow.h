#ifndef SESSIONWINDOW_H
#define SESSIONWINDOW_H

#include <QDialog>

#include "directoryparse.h"
#include "directorysearcher.h"

namespace Ui {
class SessionWindow;
}

class SessionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SessionWindow(QString mCurrentWorkingDirectory, QWidget *parent = 0);
    ~SessionWindow();

private slots:
    void on_buttonGroup_clicked();

    void WorkUpdate(QString results);

    void WorkFinished(DirectoryParse finalResult, ParseTypes::ParseAction action);

    void on_buttonIndividual_clicked();

    void on_comboGroup_currentIndexChanged(int index);

    void on_comboIndividual_currentIndexChanged(int index);

    void on_buttonEvaluation_clicked();

    void on_buttonCondition_clicked();

    void on_comboEvaluation_currentIndexChanged(int index);

private:
    Ui::SessionWindow *ui;

    QString mWorkingDirectory;

    void UserChangedSelection(const QString&);

    QThread *workerThread;

    DirectorySearcher *worker;

    DirectoryParse mCurrentDirectory;
};

#endif // SESSIONWINDOW_H
