#ifndef WIDGET_WARNING
#define WIDGET_WARNING


// Qt core
#include <QtCore/Qt>
// Qt widgets
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QGridLayout>


/**
 * Allows class to show warning message box.
*/
class WidgetWarning
{
protected:

    /** Displays warning message box.
     * 
     *  @param message text to display.
    */
    void displayWarning(QString message)
    {
        // Create message box
        QMessageBox msgBox;
        // Set text (extra spacing for better visuals) and icon
        msgBox.setText(message);
        msgBox.setIcon(QMessageBox::Warning);

        // Add spacer to message box layout for improved visuals
        QSpacerItem* spacer = new QSpacerItem(250, 50, QSizePolicy::Minimum, QSizePolicy::Minimum);
        QGridLayout* layout = qobject_cast<QGridLayout*>(msgBox.layout());
        if (layout) {
            layout->addItem(spacer, layout->rowCount()-1, 0, 1, layout->columnCount());
        }

        // Display message box
        msgBox.exec();
    }
};


#endif // WIDGET_WARNING