#-------------------------------------------------
#
# Data Tracker, Qt Port
# Copyright 2017, Shawn P. Gilroy
# Released under the GPL-V3 license
#
# Source code for project hosted at:
#
# https://github.com/miyamot0/DataTracker3
#
#-------------------------------------------------

#-------------------------------------------------
#
# Project created by QtCreator 2017-08-24T14:42:07
#
#-------------------------------------------------

TEST_FEATURES = 1

VERSION_MAJOR = 0
VERSION_MINOR = 5
VERSION_BUILD = 1

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
       "VERSION_MINOR=$$VERSION_MINOR"\
       "VERSION_BUILD=$$VERSION_BUILD"\
       "VERSION_TESTING=$$TEST_FEATURES"

QT       += core gui widgets xlsx charts network xml

TARGET = DataTracker

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    startwindow.cpp \
    sessionwindow.cpp \
    directorysearcher.cpp \
    keyseteditor.cpp \
    keysetcapture.cpp \
    recordingwindow.cpp \
    resultsdialog.cpp \
    aboutdialog.cpp \
    contactdialog.cpp \
    sessioncounter.cpp \
    licensedialog.cpp \
    reliabilitydialog.cpp \
    settingsdialog.cpp \
    sessiondurationdialog.cpp \
    faqdialog.cpp \
    filewriter.cpp \
    keyset.cpp \
    sessionviewerdialog.cpp \
    filemigrater.cpp \
    evaluationviewerdialog.cpp \
    displaytabledialog.cpp \
    seriesselectdialog.cpp \
    evaluationreportdialog.cpp \
    displaytablesdialog.cpp

HEADERS  += startwindow.h \
    sessionwindow.h \
    filetools.h \
    directoryparse.h \
    directorysearcher.h \
    parsetypes.h \
    keyseteditor.h \
    keysetcapture.h \
    keyset.h \
    keysetentry.h \
    recordingwindow.h \
    sessionevent.h \
    scoringtools.h \
    resultsdialog.h \
    aboutdialog.h \
    contactdialog.h \
    sessioncounter.h \
    licensedialog.h \
    reliabilitydialog.h \
    settingsdialog.h \
    sessiondurationdialog.h \
    reliabilityparse.h \
    reliabilityscoring.h \
    reliabilitymeasure.h \
    windowtools.h \
    faqdialog.h \
    filewriter.h \
    sessionviewerdialog.h \
    filemigrater.h \
    evaluationviewerdialog.h \
    displaytabledialog.h \
    seriesselectdialog.h \
    evaluationreportdialog.h \
    displaytablesdialog.h

FORMS    += startwindow.ui \
    sessionwindow.ui \
    keyseteditor.ui \
    keysetcapture.ui \
    recordingwindow.ui \
    resultsdialog.ui \
    aboutdialog.ui \
    contactdialog.ui \
    licensedialog.ui \
    reliabilitydialog.ui \
    settingsdialog.ui \
    sessiondurationdialog.ui \
    faqdialog.ui \
    sessionviewerdialog.ui \
    evaluationviewerdialog.ui \
    displaytabledialog.ui \
    seriesselectdialog.ui \
    evaluationreportdialog.ui \
    displaytablesdialog.ui

RESOURCES += \
    resourcefile.qrc \
    qdarkstyle/style.qrc

###
#
# Win macro's, route to appropriate directories for installation prep
#
###
win32 {
    win32:RC_ICONS += SNS.ico

    DT_FILES.files = License_Tango.txt \
                     License_QtXlsx.txt \
                     License_Qt.txt \
                     License_QDarkStyleSheet.txt \
                     License_BDataPro.txt \
                     COPYING \
                     SNS.ico \
                     source/DataTracker.tar

    CONFIG(debug, debug|release) {
        DESTDIR = $$OUT_PWD/build/debug
    } else {
        DESTDIR = $$OUT_PWD/build/release
    }

    DT_FILES.path = $$DESTDIR

    INSTALLS += DT_FILES
}

###
#
# Mac macro's, route to appropriate directories for installation prep
#
###
macx {
    macx:ICON = $${PWD}/SNS.icns

    DT_FILES.files = License_Tango.txt \
                     License_QtXlsx.txt \
                     License_Qt.txt \
                     License_QDarkStyleSheet.txt \
                     License_BDataPro.txt \
                     COPYING \
                     SNS.icns \
                     source/DataTracker.tar

    DT_FILES.path = Contents/Resources

    QMAKE_BUNDLE_DATA += DT_FILES
}

DISTFILES += \
    README.md \
    COPYING \
    License_Tango.txt \
    License_QtXlsx.txt \
    License_Qt.txt \
    License_QDarkStyleSheet.txt \
    License_BDataPro.txt \
    SNS.ico \
    source/DataTracker.tar \
    SNS.icns \
    screencaps/EvaluationViewer.png \
    screencaps/RecordingWindow.png \
    screencaps/SessionDesigner.png \
    screencaps/SessionViewer.png \
    screencaps/Reliability.png \
    screencaps/Report.png
