#ifndef KEYSETEDITOR_H
#define KEYSETEDITOR_H

#include <QDialog>

namespace Ui {
class KeySetEditor;
}

class KeySetEditor : public QDialog
{
    Q_OBJECT

public:
    explicit KeySetEditor(QWidget *parent = 0);
    ~KeySetEditor();

private:
    Ui::KeySetEditor *ui;
};

#endif // KEYSETEDITOR_H
