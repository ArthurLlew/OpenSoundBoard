#include "WidgetMessages.hpp"


void WidgetWarnings::displayWarning(QString message)
{
    // Basic warning meesagebox
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.exec();
}