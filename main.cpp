/**
   Copyright 2017 Shawn Gilroy

   This file is part of Data Tracker, Qt port.

   Data Tracker is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, version 3.

   Data Tracker is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with Data Tracker.  If not, see http://www.gnu.org/licenses/.

   The Data Tracker is a tool to assist researchers in behavior economics.

   Email: shawn(dot)gilroy(at)temple.edu

  */

#include "startwindow.h"
#include <QApplication>

#ifdef _WIN32
        #include <QSettings>
#endif

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setOrganizationName("SmallNStats");
    a.setOrganizationDomain("smallnstats.com");
    a.setApplicationName("DataTracker");

    qRegisterMetaType<DirectoryParse>("DirectoryParse");
    qRegisterMetaType<ParseTypes::ParseAction>("ParseTypes::ParseAction");

    QSettings settings;
    settings.beginGroup(QLatin1String("DTProgramSettings"));
    bool displayDark = settings.value(QLatin1String("displayDark"), false).toBool();
    settings.endGroup();

    if (displayDark)
    {
        QFile f(":qdarkstyle/style.qss");
        if (f.exists())
        {
            f.open(QFile::ReadOnly | QFile::Text);
            QTextStream ts(&f);
            a.setStyleSheet(ts.readAll());
        }
    }

    StartWindow w;
    w.show();

    return a.exec();
}
