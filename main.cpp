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


    /*
    KeySet mKeySet;

    mKeySet.KeySetName = "NameTest";
    mKeySet.TotalSeconds = 600;

    KeySetEntry f1;
    f1.KeyCode = Qt::Key_1;
    f1.KeyDescription = "SIB";
    f1.KeyName = "1";

    mKeySet.FrequencyKeys.append(f1);

    KeySetEntry d1;
    d1.KeyCode = Qt::Key_4;
    d1.KeyDescription = "SR";
    d1.KeyName = "4";

    mKeySet.DurationKeys.append(d1);


    RecordingWindow r;
    r.LoadKeys(mKeySet);
    r.show();
    */


    return a.exec();
}
