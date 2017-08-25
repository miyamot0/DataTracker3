#include "recordingwindow.h"
#include "ui_recordingwindow.h"

RecordingWindow::RecordingWindow(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RecordingWindow)
{
    ui->setupUi(this);
}

RecordingWindow::~RecordingWindow()
{
    delete ui;
}
