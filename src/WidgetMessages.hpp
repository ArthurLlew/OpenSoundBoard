#pragma once

// Qt core (defines Q_OS_WIN among other things)
#include <QtCore/Qt>
// Windows headers
#ifdef Q_OS_WIN
#define WINVER 0x0A00
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#endif
// Qt
#include <QtWidgets/QMessageBox>


/** Describes warning messageboxes that can be shown to user.*/
class WidgetWarnings
{
protected:

    /** Displays warning messagebox.
     * 
     *  @param message A message to display.
    */
    void displayWarning(QString message);
};


/** Custom message box.*/
class OSBMessageBox : public QMessageBox
{
public:

    /** Displays warning messagebox.
     * 
     *  @param message A message to display.
    */
    OSBMessageBox(QString message);
};