#pragma once

// Qt5
#include <QtCore/Qt>
#include <QtCore/QRect>
#include <QtCore/QString>
#include <QtCore/QThreadPool>
#include <QtGui/QScreen>
#include <QtGui/QCloseEvent>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QAction>
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


class MainWindow: public QMainWindow, WidgetWarnings
{
    // Geometry of the computer primary screen 
    QRect *screean_rect;

    // Central widjet of the main window
    QWidget *central_widget;
    // List of tracks (with mutex)
    QListWidget *tracks;
    // Devices tab
    QTabWidget *devices;

    // Media player managers
    MicrophonePlayerManager *microphone_player_manager;
    MediaFilesPlayerManager *mediafiles_player_manager;

    public:

    // Constructor
    MainWindow(const QApplication *app, QWidget *parent = nullptr, Qt::WindowFlags flags = Qt::WindowFlags());
    // Destructor
    ~MainWindow();

    // Safely starts portaudio (raises exception if unable to initialize)
    void start_portaudio();

    // Adds track to the list
    void add_track();
    // Opens up a dialog to select files
    QStringList open_file_dialog(QString name_filter);

    // Starts players
    void start_players(bool microphone_player, bool mediafiles_player);
    // Stops players
    void stop_players(bool *microphone_player, bool *mediafiles_player);
    // Restarts players
    void restart_players(int unused);
    // Refreshes lists of devices (also restarts portaudio to get up-to-date list and restarts players)
    void refresh_devices();
};