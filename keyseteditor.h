#ifndef KEYSETEDITOR_H
#define KEYSETEDITOR_H

#include <QDialog>
#include <QAbstractItemModel>

#include "keysetcapture.h"
#include "keyset.h"

namespace Ui {
class KeySetEditor;
}

class KeySetEditor : public QDialog
{
    Q_OBJECT

public:
    explicit KeySetEditor(QWidget *parent = 0);
    ~KeySetEditor();

    KeySet keySet;
    QString KeySetName;

private slots:
    void on_pushButtonFrequency_clicked();
    void customFrequencyMenuRequested(QPoint pos);

    void on_pushButtonDuration_clicked();
    void customDurationMenuRequested(QPoint pos);

    void on_buttonBox_clicked(QAbstractButton *button);

private:
    Ui::KeySetEditor *ui;

    KeySetCapture *keyCapture;
};

#endif // KEYSETEDITOR_H
