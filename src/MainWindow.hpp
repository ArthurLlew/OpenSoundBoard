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


/** Application main window.*/
class MainWindow: public QMainWindow, WidgetWarning
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

    /** Mouse position picked up by mousePressEvent.*/
    QPoint mouseClickedPos;

    /** App title widget.*/
    QWidget *titleBar = nullptr;
    /** List of tracks.*/
    QTableWidget *tracks = nullptr;
    /** Devices tab.*/
    QTabWidget *devices = nullptr;

    /** Microphone player manager.*/
    MicrophonePlayerWidget *microphonePlayerWidget = nullptr;
    /** Media files player manager.*/
    MediaFilesPlayerWidget *mediafilesPlayerWidget1 = nullptr;
    /** Media files player manager.*/
    MediaFilesPlayerWidget *mediafilesPlayerWidget2 = nullptr;

public:

    /** Constructor.
     * 
     *  @param app QT application.
    */
    explicit MainWindow(const QApplication *app, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    /** Destructor.*/
    ~MainWindow();

protected:

    /** Redifinition allows usage of QSS.*/
    void paintEvent(QPaintEvent *event) override;

    /** Select directory with media files.*/
    void selectDirectory();

    /** Updates devices in active players.*/
    void updateDevices();

    /** Refreshes lists of input/output devices.*/
    void refreshDevices();
};