// App embedded resources
#include "embedded.h"
// Main window widget
#include <MainWindow.hpp>
#include <QtWidgets/QSplashScreen>


int main(int argc, char *argv[])
{
    // Create application instance
    QApplication app(argc, argv);
    // Show splash image while application is loading
    QPixmap pixmap(":/resources/app.png");
    QSplashScreen splash(pixmap);
    splash.show();

    // Load style sheet
    QString styleSheet = QLatin1String(QByteArray::fromRawData((char*)style_qss, sizeof(style_qss)));
    app.setStyleSheet(styleSheet);

    // Create and show main window
    MainWindow window(&app);
    window.show();
    // Hide splash image
    splash.finish(&window);

    // Run application loop
    return app.exec();
}