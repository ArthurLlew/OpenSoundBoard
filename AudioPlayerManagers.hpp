#pragma once

// Qt5
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QThreadPool>
#include <QtCore/QRunnable>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
// Custom
#include "WidgetMessages.hpp"
#include "AudioPlayers.hpp"

using namespace std;


// Abstract class: manager for audio player
class AudioPlayerManager : public QWidget, WidgetWarnings
{
    protected:

    // Manager name
    QString name;
    // Widget main layout
    QVBoxLayout *layout;
    // Start/Stop button
    QPushButton *button_start_stop;
    // Player
    AudioPlayer *player;
    // Player state
    bool is_player_alive = false;
    // Thread pool where player will run
    QThreadPool *threadpool = new QThreadPool();

    public:

    // Constructor
    AudioPlayerManager(QString name, QWidget *parent = nullptr);
    // Destructor
    ~AudioPlayerManager();

    // Start/Stop player cycle
    void start_stop();
    // Start player
    void start();
    // Stop player
    void stop();
    // Wait for player to stop
    void wait_player();

    // Display player error
    void player_error(QString message);
};


// Manages microphone player
class MicrophonePlayerManager : public AudioPlayerManager
{
    public:

    // Constructor
    MicrophonePlayerManager(QTabWidget *devices, QString name, QWidget *parent = nullptr);
};


// Manages media files player
class MediaFilesPlayerManager : public AudioPlayerManager
{
    public:

    // Constructor
    MediaFilesPlayerManager(QTabWidget *devices, QListWidget *tracks, QString name, QWidget *parent = nullptr);

    // Start player
    void media_play_pause();
    // Stop player
    void media_stop();
};