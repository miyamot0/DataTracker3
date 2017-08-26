#include "startwindow.h"
#include <QApplication>

#include "directoryparse.h"
#include "parsetypes.h"

#include "keyseteditor.h"
#include "keysetcapture.h"

#include "recordingwindow.h"
#include "scoringtools.h"
#include "filetools.h"
#include "resultsdialog.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    qRegisterMetaType<DirectoryParse>("DirectoryParse");
    qRegisterMetaType<ParseTypes::ParseAction>("ParseTypes::ParseAction");

    StartWindow w;
    w.show();

    KeySet mKeySet;

    mKeySet.KeySetName = "NameTest";
    mKeySet.TotalSeconds = 25;
    mKeySet.Session = 1;

    KeySetEntry f1;
    f1.KeyCode = Qt::Key_1;
    f1.KeyDescription = "SIB";
    f1.KeyName = "1";

    KeySetEntry f2;
    f2.KeyCode = Qt::Key_2;
    f2.KeyDescription = "AGG";
    f2.KeyName = "2";

    KeySetEntry f3;
    f3.KeyCode = Qt::Key_3;
    f3.KeyDescription = "DIS";
    f3.KeyName = "3";

    KeySetEntry f4;
    f4.KeyCode = Qt::Key_4;
    f4.KeyDescription = "DA";
    f4.KeyName = "4";

    mKeySet.FrequencyKeys.append(f1);
    mKeySet.FrequencyKeys.append(f2);
    mKeySet.FrequencyKeys.append(f3);
    mKeySet.FrequencyKeys.append(f4);

    KeySetEntry d1;
    d1.KeyCode = Qt::Key_R;
    d1.KeyDescription = "RRR";
    d1.KeyName = "R";

    KeySetEntry d2;
    d2.KeyCode = Qt::Key_Q;
    d2.KeyDescription = "QQQ";
    d2.KeyName = "Q";

    KeySetEntry d3;
    d3.KeyCode = Qt::Key_T;
    d3.KeyDescription = "TTT";
    d3.KeyName = "T";

    mKeySet.DurationKeys.append(d1);
    mKeySet.DurationKeys.append(d2);
    mKeySet.DurationKeys.append(d3);

    RecordingWindow r;
    r.LoadKeys(mKeySet);
    r.SetGroup("Participants");
    r.SetIndividual("Shawn");
    r.SetEvaluation("FA");
    r.SetCondition("Control");
    r.SetCollector("ABC");
    r.SetRole("Primary");
    r.exec();

    ResultsDialog mResults;

    ScoringTools::ScoreOverallSchedule(&r.PressedKeys, &mKeySet, &r.startTime, &r.endTime,
                                       &mResults.FrequencyOverall, &mResults.DurationOverall, &mResults.TimeOverall);

    ScoringTools::ScoreSpecificSchedule(&r.PressedKeys, &mKeySet, &r.endTime, Schedule::One,
                                        &mResults.FrequencyOne, &mResults.DurationOne, &mResults.TimeOne);

    ScoringTools::ScoreSpecificSchedule(&r.PressedKeys, &mKeySet, &r.endTime, Schedule::Two,
                                        &mResults.FrequencyTwo, &mResults.DurationTwo, &mResults.TimeTwo);

    ScoringTools::ScoreSpecificSchedule(&r.PressedKeys, &mKeySet, &r.endTime, Schedule::Three,
                                        &mResults.FrequencyThree, &mResults.DurationThree, &mResults.TimeThree);

    mResults.SetKeySet(mKeySet);

    mResults.BuildTables();
    mResults.BuildNarrative(&r.PressedKeys, &r.startTime);
    mResults.SetParameters("Participants", "Shawn", "FA",
                           "Control", "ABC", "keySet",
                           "BA", "Primary");

    mResults.BuildJson(&r.PressedKeys, &r.startTime, FileTools::pathAppend(QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation)[0], "DataTracker3"));
    mResults.BuildPlot(mKeySet, &r.PressedKeys, &r.startTime, &r.endTime);

    mResults.show();

    return a.exec();
}
