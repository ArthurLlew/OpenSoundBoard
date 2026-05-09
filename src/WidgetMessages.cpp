#include <WidgetMessages.hpp>


void WidgetWarnings::displayWarning(QString message)
{
    // Warning meesagebox
    OSBMessageBox msgBox(message);
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
}


OSBMessageBox::OSBMessageBox(QString message) : QMessageBox()
{
    // Set message and icon
    setText(message);
}