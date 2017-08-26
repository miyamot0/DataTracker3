#-------------------------------------------------
#
# Project created by QtCreator 2017-08-24T14:42:07
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

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
    recordingwindow.cpp

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
    sessionevent.h

FORMS    += startwindow.ui \
    sessionwindow.ui \
    keyseteditor.ui \
    keysetcapture.ui \
    recordingwindow.ui
