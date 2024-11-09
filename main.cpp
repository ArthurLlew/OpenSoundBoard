#include "MainWindow.hpp"


int main(int argc, char **argv)
{
    // Create application instance
    QApplication app(argc, argv);

    // Load style sheet
    //QFile file("style.qss");
    //file.open(QFile::ReadOnly);
    //QString styleSheet = QLatin1String(file.readAll());
    //app.setStyleSheet(styleSheet);

    // Create and show main window
    MainWindow window(&app);
    window.show();

    // Run application loop
    return app.exec();
}