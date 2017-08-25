#include "keyseteditor.h"
#include "ui_keyseteditor.h"

KeySetEditor::KeySetEditor(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeySetEditor)
{
    ui->setupUi(this);
}

KeySetEditor::~KeySetEditor()
{
    delete ui;
}
