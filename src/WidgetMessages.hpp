#pragma once

// Qt core (defines Q_OS_WIN among other things)
#include <QtCore/Qt>
// Qt
#include <QtWidgets/QMessageBox>


/** Implements warning messagebox functionality.*/
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