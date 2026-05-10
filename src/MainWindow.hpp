#pragma once


// Qt core
#include <QtCore/Qt>
#include <QtCore/QRect>
#include <QtCore/QString>
// Qt GUI
#include <QtGui/QScreen>
#include <QtGui/QAction>
#include <QtGui/QMouseEvent>
// Qt widgets
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QAbstractItemView>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTabWidget>
// Message boxes
#include <WidgetMessageBoxing/WidgetWarning.cpp>
// Device tab widget
#include <DeviceTab.hpp>
// Audio track widget
#include <AudioTrack.hpp>
// Microphone rerouter widget
#include <AudioPlayerWidgets/MicrophonePlayerWidget.hpp>
// Media files player widget
#include <AudioPlayerWidgets/MediaFilesPlayerWidget.hpp>


/**
 * Main window widget of application.
 */
class MainWindow: public QMainWindow, WidgetWarning
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

    // List of tracks.
    QTableWidget *tracks = nullptr;
    // Devices tab.
    QTabWidget *devices = nullptr;

    // Microphone player manager.
    MicrophonePlayerWidget *microphonePlayerWidget = nullptr;
    // Media files player manager 1.
    MediaFilesPlayerWidget *mediafilesPlayerWidget1 = nullptr;
    // Media files player manager 2.
    MediaFilesPlayerWidget *mediafilesPlayerWidget2 = nullptr;

public:

    /**
     * Constructor.
     * 
     * @param app Qt application
    */
    explicit MainWindow(const QApplication *app, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    /**
     * Destructor.
     */
    ~MainWindow();

protected:

    /**
     * Reimplemented to allow usage of QSS.
     */
    void paintEvent(QPaintEvent *event) override;

    /**
     * Opens directory (via file dialog) and loads info about all media files in it.
     */
    void selectDirectory();

    /**
     * Updates list of audio devices in media players.
     */
    void updateDevices();

    /**
     * Updates list of audio devices.
     */
    void refreshDevices();
};