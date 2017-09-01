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

#ifndef WINDOWTOOLS_H
#define WINDOWTOOLS_H

#include <QMainWindow>

class WindowTools
{
public:

/**
 * @brief SetWindowFixed
 * @param mWindow
 */
void static SetWindowFixed(QMainWindow * window)
{
    #ifdef _WIN32

    window->setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);

    #elif TARGET_OS_MAC

    window->setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);

    #endif
}

/**
 * @brief SetDialogFixed
 * @param window
 */
void static SetDialogFixed(QDialog * window)
{
    #ifdef _WIN32

    window->setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);

    #elif TARGET_OS_MAC

    window->setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);

    #endif
}

/**
 * @brief SetDialogFixedDisplay
 * @param window
 */
void static SetDialogFixedDisplay(QDialog * window)
{
    #ifdef _WIN32

    window->setWindowFlags(Qt::Window |
                           Qt::MSWindowsFixedSizeDialogHint |
                           Qt::WindowTitleHint |
                           Qt::WindowSystemMenuHint |
                           Qt::CustomizeWindowHint |
                           Qt::WindowCloseButtonHint);

    #elif TARGET_OS_MAC

    window->setWindowFlags(Qt::Window |
                           Qt::MSWindowsFixedSizeDialogHint |
                           Qt::WindowTitleHint |
                           Qt::WindowSystemMenuHint |
                           Qt::CustomizeWindowHint |
                           Qt::WindowCloseButtonHint);

    #endif
}

/**
 * @brief SetDialogFixedMaximize
 * @param window
 */
void static SetDialogFixedMaximize(QDialog * window)
{
    #ifdef _WIN32

    Qt::WindowFlags flags = window->windowFlags();

    flags |= Qt::WindowMaximizeButtonHint;
    flags & ~Qt::WindowContextHelpButtonHint;

    window->setWindowFlags(flags);

    #elif TARGET_OS_MAC

    Qt::WindowFlags flags = window->windowFlags();

    flags |= Qt::WindowMaximizeButtonHint;
    flags & ~Qt::WindowContextHelpButtonHint;

    window->setWindowFlags(flags);

    #endif
}

};

#endif // WINDOWTOOLS_H
