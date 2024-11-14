#pragma once

// Qt5
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