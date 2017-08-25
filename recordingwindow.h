#ifndef RECORDINGWINDOW_H
#define RECORDINGWINDOW_H

#include <QDialog>

namespace Ui {
class RecordingWindow;
}

class RecordingWindow : public QDialog
{
    Q_OBJECT

public:
    explicit RecordingWindow(QWidget *parent = 0);
    ~RecordingWindow();

private:
    Ui::RecordingWindow *ui;
};

#endif // RECORDINGWINDOW_H
