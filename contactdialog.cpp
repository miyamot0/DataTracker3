#include "contactdialog.h"
#include "ui_contactdialog.h"

ContactDialog::ContactDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContactDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("Contact"));
}

ContactDialog::~ContactDialog()
{
    delete ui;
}
