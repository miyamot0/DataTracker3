#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QMainWindow>

namespace Ui {
class StartWindow;
}

class StartWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit StartWindow(QWidget *parent = 0);
    ~StartWindow();

private slots:
    void on_actionExit_triggered();

    void on_actionCalculate_Reliability_triggered();

    void on_actionSettings_2_triggered();

    void on_actionContact_triggered();

    void on_actionAbout_triggered();

    void on_buttonStart_clicked();

private:
    Ui::StartWindow *ui;
};

#endif // STARTWINDOW_H
