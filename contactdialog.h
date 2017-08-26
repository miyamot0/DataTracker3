#ifndef CONTACTDIALOG_H
#define CONTACTDIALOG_H

#include <QDialog>

namespace Ui {
class ContactDialog;
}

class ContactDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContactDialog(QWidget *parent = 0);
    ~ContactDialog();

private:
    Ui::ContactDialog *ui;
};

#endif // CONTACTDIALOG_H
