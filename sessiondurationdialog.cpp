#include "sessiondurationdialog.h"
#include "ui_sessiondurationdialog.h"

SessionDurationDialog::SessionDurationDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SessionDurationDialog)
{
    ui->setupUi(this);
}

int SessionDurationDialog::GetSeconds()
{
    int seconds = ui->spinBox->value();

    if (ui->comboBox->currentIndex() == 0)
    {
        return seconds * 60 * 60;
    }
    else if (ui->comboBox->currentIndex() == 1)
    {
        return seconds * 60;
    }
    else if (ui->comboBox->currentIndex() == 2)
    {
        return seconds;
    }
    else
    {
        return seconds;
    }
}

void SessionDurationDialog::done(int r)
{
    if (QDialog::Accepted == r)
    {
        AcceptDialog = true;
    }

    QDialog::done(r);
}

SessionDurationDialog::~SessionDurationDialog()
{
    delete ui;
}
