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
    AudioPlayerManager(AudioPlayer *player, QString name, QWidget *parent = nullptr);
    // Destructor
    ~AudioPlayerManager();

    protected:

    // Display player error
    void player_error(QString message);

    // Start/Stop player
    void player_run_stop();

    public:

    // Returns player state
    bool player_state();
    // Start player
    void player_run();
    // Stop player
    void player_stop();
    // Wait for the player to stop
    void player_wait();

    Q_OBJECT
    signals:
    // Ask player to stop
    void ask_player_stop();
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

    private:

    // Display player error and update track state
    void player_error(QString message);

    // Sets track volume
    void set_volume(int value);
    // Updates track progress
    void update_progress();

    // Stops audio track
    void track_stop();
    // Audio track play-pause
    void track_play_pause();
    // Update track state to STOPPED
    void player_track_ended();

    public:

    // Insert new track
    void track_insert(QString filepath, QString name);

    Q_OBJECT
    signals:
    // Ask player to set new track
    void ask_new_track (QString filepath);
    // Ask player to change track state
    void ask_new_track_state(TrackState track_state);
};