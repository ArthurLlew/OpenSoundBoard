#pragma once

// Qt5
#include <QtCore/Qt>
#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtCore/QThreadPool>
#include <QtGui/QScreen>
#include <QtGui/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QFileDialog>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QAbstractItemView>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTabWidget>
// PortAudio
#include <portaudio.h>
// Custom
#include "WidgetMessages.hpp"
#include "DeviceTab.hpp"
#include "AudioTrack.hpp"
#include "AudioPlayerManagers.hpp"

using namespace std;


/** Application main window.*/
class MainWindow: public QMainWindow, WidgetWarnings
{
    /** Geometry of the computer's primary screen.*/
    QRect *screeanGeometry;

    /** List of tracks.*/
    QListWidget *tracks;
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
    MainWindow(const QApplication *app, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    /** Destructor.*/
    ~MainWindow();

    protected:

    /** Safely starts portaudio (raises exception if unable to initialize).*/
    void startPortaudio();

    /** Select directory with media files.*/
    void selectDirectory();

    /** Starts players.
     * 
     *  @param microphone_player previous microphone player state.
     * 
     *  @param mediafiles_player1 previous mediafiles player state.
     * 
     *  @param mediafiles_player2 previous mediafiles player state.
    */
    void startPlayers(bool microphone_player, bool mediafiles_player1, bool mediafiles_player2);
    /** Stops players.
     * 
     *  @param microphone_player where to store current microphone player state.
     * 
     *  @param mediafiles_player1 where to store current mediafiles player state.
     * 
     *  @param mediafiles_player2 where to store current mediafiles player state.
    */
    void stopPlayers(bool *microphone_player, bool *mediafiles_player1, bool *mediafiles_player2);
    /** Restarts players.*/
    void restartPlayers(int unused);
    /** Refreshes lists of devices (also restarts portaudio to get up-to-date list and restarts players).*/
    void refreshDevices();
};