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
#include <QtWidgets/QProgressbar>
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

    // Thread pool where player will run
    QThreadPool *threadpool = new QThreadPool();
    // Player
    AudioPlayer *player;
    // Player state
    bool is_player_alive = false;

    public:

    // Constructor
    AudioPlayerManager(QString name, QWidget *parent = nullptr);
    // Destructor
    ~AudioPlayerManager();

    // Start player
    void run();
    // Stop player
    void kill();
    // Wait for player to stop
    void wait_player();
    // Start/Stop player cycle
    void run_kill();

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
    Q_OBJECT

    // Track name label
    QLabel *track_name;
    // Play button
    QPushButton *button_play;
    // Track progress bar
    QProgressBar *progress;
    // Volume label
    QLabel *volume_label;

    // Volume adjustments
    float volume;
    // Track duration info
    int duration_cur = 0;
    int duration_total = 0;
    // Tells track current state
    TrackState track_state = STOPPED;

    public:

    // Constructor
    MediaFilesPlayerManager(QTabWidget *devices, QString name, QRect *screean_rect, QWidget *parent = nullptr);

    // Display player error and update track state
    void player_error(QString message);
    // Update track state to STOPPED
    void player_track_ended();

    // Sets track volume
    void set_volume(int value);
    // Updates track progress
    void update_progress();

    // Insert new track
    void insert_track(QString filepath, QString name);

    // Stops audio track
    void stop();
    // Audio track play-pause cycle
    void play_pause();

    signals:
    // Ask player to set new track
    void ask_new_track (QString filepath);
    // Ask player to change track state
    void ask_new_track_state(TrackState track_state);
};