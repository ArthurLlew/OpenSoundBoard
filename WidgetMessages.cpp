#include "WidgetMessages.hpp"


void WidgetWarnings::show_warning(QString message)
{
    // Basic warning meesagebox
    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.exec();
}