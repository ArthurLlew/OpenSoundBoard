#pragma once

// Qt core (defines Q_OS_WIN among other things)
#include <QtCore/Qt>
// Windows headers
#ifdef Q_OS_WIN
#define WINVER 0x0A00
#include <windows.h>
#include <windowsx.h>
#include <dwmapi.h>
#endif
// Qt
#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtCore/QThreadPool>
#include <QtGui/QScreen>
#include <QtGui/QAction>
#include <QtGui/QMouseEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QAbstractItemView>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
// Custom
#include <WidgetMessages.hpp>
#include <DeviceTab.hpp>
#include <AudioTrack.hpp>
#include <AudioPlayerManagers/MicrophonePlayerManager.hpp>
#include <AudioPlayerManagers/MediaFilesPlayerManager.hpp>


/** Application main window.*/
class MainWindow: public QMainWindow, WidgetWarnings
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

    /** Geometry of the computer's primary screen.*/
    QRect *screeanGeometry = nullptr;
    /** Mouse position picked up by mousePressEvent.*/
    QPoint mouseClickedPos;

    /** App title widget.*/
    QWidget *titleBar = nullptr;
    /** List of tracks.*/
    QTableWidget *tracks = nullptr;
    /** Devices tab.*/
    QTabWidget *devices = nullptr;

    /** Microphone player manager.*/
    MicrophonePlayerManager *microphonePlayerManager = nullptr;
    /** Media files player manager.*/
    MediaFilesPlayerManager *mediafilesPlayerManager1 = nullptr;
    /** Media files player manager.*/
    MediaFilesPlayerManager *mediafilesPlayerManager2 = nullptr;

public:

    /** Constructor.
     * 
     *  @param app QT application.
    */
    explicit MainWindow(const QApplication *app, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    /** Destructor.*/
    ~MainWindow();

protected:

    /** Handles paint event.
     * 
     *  @param event event info.
    */
    void paintEvent(QPaintEvent *event) override;
    /** Handles native events (for example, allows to handle resizing).
     * 
     *  @param eventType event type.
     *  @param message event info.
     *  @param result method handling result
     * 
     *  @return True if the event was handled, otherwise false.
    */
    bool nativeEvent(const QByteArray &, void *message, qintptr *result) override;

    /** Select directory with media files.*/
    void selectDirectory();

    /** Updates devices in active players.*/
    void updateDevices();

    /** Refreshes lists of input/output devices.*/
    void refreshDevices();
};