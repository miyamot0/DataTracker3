#include "keysetcapture.h"
#include "ui_keysetcapture.h"

KeySetCapture::KeySetCapture(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::KeySetCapture)
{
    ui->setupUi(this);
}

KeySetCapture::~KeySetCapture()
{
    delete ui;
}
