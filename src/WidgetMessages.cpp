#include "WidgetMessages.hpp"


void WidgetWarnings::displayWarning(QString message)
{
    printf("%s\n", message.toStdString().c_str());
    // Basic warning meesagebox
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.exec();
}