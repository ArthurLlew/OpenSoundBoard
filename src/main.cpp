// App embedded resources
#include "embedded.h"
// Main window widget
#include <MainWindow.hpp>


int main(int argc, char *argv[])
{
    // Create application instance
    QApplication app(argc, argv);

    // Load style sheet
    QString styleSheet = QLatin1String(QByteArray::fromRawData((char*)style_qss, sizeof(style_qss)));
    app.setStyleSheet(styleSheet);

    // Create and show main window
    MainWindow window(&app);
    window.show();

    // Run application loop
    return app.exec();
}