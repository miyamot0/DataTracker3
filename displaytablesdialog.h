#ifndef DISPLAYTABLESDIALOG_H
#define DISPLAYTABLESDIALOG_H

#include <QDialog>
#include <QMenu>
#include <QMenuBar>

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

private:
    Ui::DisplayTablesDialog *ui;

    QMenuBar *menuBar;
    QMenu *fileMenu;
    QMenu *closeMenu;

    QAction *saveAction;
    QAction *closeAction;

    void SaveToFile();
};

#endif // DISPLAYTABLESDIALOG_H
