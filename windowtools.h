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



    #endif
}

void static SetDialogFixed(QDialog * window)
{
    #ifdef _WIN32

    window->setWindowFlags(Qt::Window | Qt::MSWindowsFixedSizeDialogHint);

    #elif TARGET_OS_MAC



    #endif
}

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



    #endif
}

};

#endif // WINDOWTOOLS_H
