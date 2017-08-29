#ifndef FAQDIALOG_H
#define FAQDIALOG_H

#include <QDialog>

namespace Ui {
class FAQDialog;
}

class FAQDialog : public QDialog
{
    Q_OBJECT

public:
    explicit FAQDialog(QWidget *parent = 0);
    ~FAQDialog();

private:
    Ui::FAQDialog *ui;
};

#endif // FAQDIALOG_H
