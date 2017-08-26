#include "startwindow.h"
#include <QApplication>

#include "directoryparse.h"
#include "parsetypes.h"

#include "keyseteditor.h"
#include "keysetcapture.h"

#include "recordingwindow.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<DirectoryParse>("DirectoryParse");
    qRegisterMetaType<ParseTypes::ParseAction>("ParseTypes::ParseAction");

    StartWindow w;
    w.show();


    KeySet mKeySet;

    mKeySet.KeySetName = "NameTest";
    mKeySet.TotalSeconds = 600;

    KeySetEntry f1;
    f1.KeyCode = Qt::Key_1;
    f1.KeyDescription = "SIB";
    f1.KeyName = "1";

    KeySetEntry f2;
    f2.KeyCode = Qt::Key_2;
    f2.KeyDescription = "AGG";
    f2.KeyName = "2";

    mKeySet.FrequencyKeys.append(f1);
    mKeySet.FrequencyKeys.append(f2);

    KeySetEntry d1;
    d1.KeyCode = Qt::Key_R;
    d1.KeyDescription = "R";
    d1.KeyName = "R";

    KeySetEntry d2;
    d2.KeyCode = Qt::Key_Q;
    d2.KeyDescription = "Q";
    d2.KeyName = "Q";

    mKeySet.DurationKeys.append(d1);
    mKeySet.DurationKeys.append(d2);

    RecordingWindow r;
    r.LoadKeys(mKeySet);
    r.SetGroup("Group");
    r.SetIndividual("Individual");
    r.SetEvaluation("Functional Analysis");
    r.SetCondition("Control");
    r.SetCollector("ABC");
    r.SetRole("Primary");
    r.show();

    return a.exec();
}
