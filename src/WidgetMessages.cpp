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

    #ifdef Q_OS_WIN
    // Set darkmode for Windows
    BOOL use_dark_mode = true;
    DwmSetWindowAttribute(HWND(winId()), DWMWINDOWATTRIBUTE::DWMWA_USE_IMMERSIVE_DARK_MODE,
                            &use_dark_mode, sizeof(use_dark_mode));
    #endif
}