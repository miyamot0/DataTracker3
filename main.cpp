#include "startwindow.h"
#include <QApplication>

#include "directoryparse.h"
#include "parsetypes.h"




#include "keyseteditor.h"

#include "keysetcapture.h"



int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<DirectoryParse>("DirectoryParse");
    qRegisterMetaType<ParseTypes::ParseAction>("ParseTypes::ParseAction");

    StartWindow w;
    w.show();

    KeySetEditor keyEditor;
    keyEditor.show();

    return a.exec();
}
