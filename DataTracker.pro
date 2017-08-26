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
VERSION_MINOR = 0
VERSION_BUILD = 1

DEFINES += "VERSION_MAJOR=$$VERSION_MAJOR"\
       "VERSION_MINOR=$$VERSION_MINOR"\
       "VERSION_BUILD=$$VERSION_BUILD"\
       "VERSION_TESTING=$$TEST_FEATURES"

QT       += core gui xlsx charts widgets

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
    sessioncounter.cpp

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
    sessioncounter.h

FORMS    += startwindow.ui \
    sessionwindow.ui \
    keyseteditor.ui \
    keysetcapture.ui \
    recordingwindow.ui \
    resultsdialog.ui \
    aboutdialog.ui \
    contactdialog.ui

DISTFILES += \
    README.md \
    COPYING \
    License_Tango.txt \
    License_QtXlsx.txt \
    License_Qt.txt \
    SNS.ico

RESOURCES += \
    resourcefile.qrc

###
#
# Win macro's, route to appropriate directories for installation prep
#
###
win32 {
    win32:RC_ICONS += SNS.ico

    DT_FILES.files = License_Qt.txt \
                     License_Tango.txt \
                     License_QtXlsx.txt \
                     COPYING \
                     SNS.ico

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

    DT_FILES.files = License_Qt.txt \
                     License_Tango.txt \
                     License_QtXlsx.txt \
                     COPYING \
                     SNS.icns

    DT_FILES.path = Contents/Resources

    QMAKE_BUNDLE_DATA += DT_FILES
}
