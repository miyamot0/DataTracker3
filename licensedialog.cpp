#include "licensedialog.h"
#include "ui_licensedialog.h"

#include <QFile>
#include <QTextStream>

LicenseDialog::LicenseDialog(QString fileName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LicenseDialog)
{
    ui->setupUi(this);

    QFile fileRef(fileName);

    if (fileRef.open(QFile::ReadOnly | QFile::Text))
    {
        QTextStream inputTextStream(&fileRef);

        ui->textBrowser->setText(inputTextStream.readAll());
    }
}

LicenseDialog::~LicenseDialog()
{
    delete ui;
}
