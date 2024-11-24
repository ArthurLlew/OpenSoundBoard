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
#include "WidgetMessages.hpp"
#include "DeviceTab.hpp"
#include "AudioTrack.hpp"
#include "AudioPlayerManagers.hpp"


/** Application main window.*/
class MainWindow: public QMainWindow, WidgetWarnings
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

    /** Geometry of the computer's primary screen.*/
    QRect *screeanGeometry;
    /** Mouse position picked up by mousePressEvent.*/
    QPoint mouseClickedPos;

    /** App title widget.*/
    QWidget *titleBar;
    /** List of tracks.*/
    QTableWidget *tracks;
    /** Devices tab.*/
    QTabWidget *devices;

    /** Microphone player manager.*/
    MicrophonePlayerManager *microphonePlayerManager;
    /** Media files player manager.*/
    MediaFilesPlayerManager *mediafilesPlayerManager1;
    /** Media files player manager.*/
    MediaFilesPlayerManager *mediafilesPlayerManager2;

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

    /** Starts players.
     * 
     *  @param microphone_player previous microphone player state.
     *  @param mediafiles_player1 previous mediafiles player state.
     *  @param mediafiles_player2 previous mediafiles player state.
    */
    void startPlayers(bool microphone_player, bool mediafiles_player1, bool mediafiles_player2);
    /** Stops players.
     * 
     *  @param microphone_player where to store current microphone player state.
     *  @param mediafiles_player1 where to store current mediafiles player state.
     *  @param mediafiles_player2 where to store current mediafiles player state.
    */
    void stopPlayers(bool *microphone_player, bool *mediafiles_player1, bool *mediafiles_player2);
    /** Restarts players.*/
    void restartPlayers(int unused);
    /** Refreshes lists of devices (also restarts portaudio to get up-to-date list and restarts players).*/
    void refreshDevices();
};



static LONG_PTR savedWndProc;