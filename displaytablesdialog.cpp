#include "displaytablesdialog.h"
#include "ui_displaytablesdialog.h"

#include <QFileDialog>
#include <QDir>

#include "windowtools.h"

DisplayTablesDialog::DisplayTablesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DisplayTablesDialog)
{
    ui->setupUi(this);

    setWindowTitle(tr("Results Table"));

    saveAction = new QAction(tr("&Save"));
    saveAction->setShortcuts(QKeySequence::Save);
    saveAction->setStatusTip(tr("Save results to file"));

    closeAction = new QAction(tr("&Close"));
    closeAction->setShortcuts(QKeySequence::Close);

    connect(saveAction, &QAction::triggered, this, &DisplayTablesDialog::SaveToFile);
    connect(closeAction, &QAction::triggered, this, &DisplayTablesDialog::close);

    menuBar = new QMenuBar(0);

    fileMenu = menuBar->addMenu(tr("&File"));
    fileMenu->addAction(saveAction);
    fileMenu->addSeparator();
    fileMenu->addAction(closeAction);

    layout()->setMenuBar(menuBar);

    WindowTools::SetDialogFixedMaximize(this);
}

/**
 * @brief DisplayTablesDialog::SaveToFile
 */
void DisplayTablesDialog::SaveToFile()
{
    QString file_name;
    QString fileFilter = "Spreadsheet (*.xlsx)";

#ifdef _WIN32

        file_name = QFileDialog::getSaveFileName(this, tr("Save spreadsheet file"), QDir::homePath(),
                                         fileFilter);

#elif TARGET_OS_MAC

        file_name = QFileDialog::getSaveFileName(this, tr("Save spreadsheet file"), QDir::homePath(),
                                         fileFilter, &fileFilter, QFileDialog::Option::DontUseNativeDialog);

        if (!file_name.contains(".xlsx"))
        {
            file_name.append(".xlsx");
        }

#endif

    if(!file_name.trimmed().isEmpty())
    {
        /*

        QApplication::setOverrideCursor(Qt::WaitCursor);

        QXlsx::Document xlsx;

        int rows = ui->tableWidget->rowCount();
        int cols = ui->tableWidget->columnCount();

        QString temp;

        for (int i=0; i<rows; i++)
        {
            for (int j=0; j<cols; j++)
            {
                QTableWidgetItem *item = ui->tableWidget->item(i, j);

                if (item != NULL && !item->text().isEmpty())
                {
                    temp = ui->tableWidget->item(i, j)->data(Qt::DisplayRole).toString();
                    xlsx.write(i + 1, j + 1, temp);
                }
            }
        }

        xlsx.saveAs(file_name);

        QApplication::restoreOverrideCursor();

        */
    }
}

DisplayTablesDialog::~DisplayTablesDialog()
{
    delete ui;
}

/**
 * @brief DisplayTablesDialog::InsertData
 * @param data
 */
void DisplayTablesDialog::InsertData(QList<QList<QStringList>> * data)
{
    QList<QTableWidget *> mTables;
    mTables.append(ui->tableWidgetOverall);
    mTables.append(ui->tableWidgetOne);
    mTables.append(ui->tableWidgetTwo);
    mTables.append(ui->tableWidgetThree);

    for (int i(0); i<mTables.count(); i++)
    {
        mTables.at(i)->clear();
        mTables.at(i)->setRowCount(0);
        mTables.at(i)->setColumnCount(0);

        QStringList temp;

        for (int tableNumberRows(0); tableNumberRows < data->at(i).count(); tableNumberRows++)
        {
            temp = data->at(i).at(tableNumberRows);
            mTables.at(i)->insertRow(mTables.at(i)->rowCount());

            for (int tableNumberCols(0); tableNumberCols < data->at(i).at(tableNumberRows).count(); tableNumberCols++)
            {
                if(tableNumberCols >= mTables.at(i)->columnCount())
                {
                    mTables.at(i)->insertColumn(mTables.at(i)->columnCount());
                }

                // Less one to adjust for Excel-style coordinates
                mTables.at(i)->setItem(tableNumberRows - 1, tableNumberCols - 1, new QTableWidgetItem(temp.at(tableNumberCols)));
            }
        }
    }
}
