#ifndef KEYSETCAPTURE_H
#define KEYSETCAPTURE_H

#include <QDialog>
#include <QKeyEvent>
#include <QAbstractButton>

namespace Ui {
class KeySetCapture;
}

class KeySetCapture : public QDialog
{
    Q_OBJECT

public:
    explicit KeySetCapture(QWidget *parent = 0);

    QKeyEvent *mKey;

    bool enteredKey = false;

    QString KeyDescription;
    QString KeyText;
    int KeyCode;

    ~KeySetCapture();

private slots:
    void on_buttonKey_clicked();

    void on_buttonBox_clicked(QAbstractButton *button);

protected:
    bool eventFilter(QObject *, QEvent *e);

private:
    Ui::KeySetCapture *ui;

    bool waitingOnInput = false;
};

#endif // KEYSETCAPTURE_H
