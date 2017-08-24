#ifndef SESSIONWINDOW_H
#define SESSIONWINDOW_H

#include <QDialog>

namespace Ui {
class SessionWindow;
}

class SessionWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SessionWindow(QWidget *parent = 0);
    ~SessionWindow();

private:
    Ui::SessionWindow *ui;
};

#endif // SESSIONWINDOW_H
