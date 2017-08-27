#ifndef SESSIONDURATIONDIALOG_H
#define SESSIONDURATIONDIALOG_H

#include <QDialog>

namespace Ui {
class SessionDurationDialog;
}

class SessionDurationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SessionDurationDialog(QWidget *parent = 0);
    int GetSeconds();
    bool AcceptDialog = false;
    ~SessionDurationDialog();

private:
    Ui::SessionDurationDialog *ui;

    void done(int r);
};

#endif // SESSIONDURATIONDIALOG_H
