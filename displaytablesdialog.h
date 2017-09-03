#ifndef DISPLAYTABLESDIALOG_H
#define DISPLAYTABLESDIALOG_H

#include <QDialog>

namespace Ui {
class DisplayTablesDialog;
}

class DisplayTablesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit DisplayTablesDialog(QWidget *parent = 0);

    void InsertData(QList<QList<QStringList>> * data);
    ~DisplayTablesDialog();

private slots:
    void on_pushButtonSave_clicked();

private:
    Ui::DisplayTablesDialog *ui;
};

#endif // DISPLAYTABLESDIALOG_H
