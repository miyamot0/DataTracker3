#ifndef KEYSETCAPTURE_H
#define KEYSETCAPTURE_H

#include <QDialog>

namespace Ui {
class KeySetCapture;
}

class KeySetCapture : public QDialog
{
    Q_OBJECT

public:
    explicit KeySetCapture(QWidget *parent = 0);
    ~KeySetCapture();

private:
    Ui::KeySetCapture *ui;
};

#endif // KEYSETCAPTURE_H
