#pragma once

// Qt5
#include <QtCore/QObject>
#include <QtCore/QMimeData>
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


/** Audio player manager widget. Can run, stop and wait provided audio player.*/
class AudioPlayerManager : public QWidget, WidgetWarnings
{
    protected:

    /** Manager name.*/
    QString name;
    /** Widget main layout.*/
    QVBoxLayout *layout;
    /** Start/Stop button.*/
    QPushButton *buttonStartStop;

    /** Thread pool where player will run.*/
    QThreadPool *threadpool;
    /** Audio player.*/
    AudioPlayer *player;
    /** Audio player state.*/
    bool isPlayerAlive = false;

    public:

    /** Constructor.
     * 
     *  @param player Audio player.
     * 
     *  @param name player name.
    */
    AudioPlayerManager(AudioPlayer *player, QString name, QWidget *parent = nullptr);
    /** Destructor.*/
    ~AudioPlayerManager();

    protected:

    /** Display player error.
     * 
     *  @param message error message.
    */
    void playerError(QString message);

    /** Start/Stop player.*/
    void player_run_stop();

    public:

    /** Returns player state.*/
    bool player_state();
    /** Start player.*/
    void player_run();
    /** Stop player.*/
    void player_stop();
    /** Wait for the player to stop.*/
    void player_wait();

    Q_OBJECT
    signals:
    /** Ask player to stop.*/
    void ask_player_stop();
};


/** Microphone player manager.*/
class MicrophonePlayerManager : public AudioPlayerManager
{
    public:

    /** Constructor.
     * 
     *  @param player Audio player.
     * 
     *  @param name player name.
    */
    MicrophonePlayerManager(QTabWidget *devices, QString name, QWidget *parent = nullptr);
};


/** Media files player manager.*/
class MediaFilesPlayerManager : public AudioPlayerManager
{
    /** Track name label.*/
    QLabel *trackName;
    /** Play button.*/
    QPushButton *buttonPlay;
    /** Track progress bar.*/
    QProgressBar *progress;
    /** Volume label.*/
    QLabel *volumeLabel;

    /** Track volume.*/
    float volume;
    /** Track current duration.*/
    int duration_cur = 0;
    /** Track total duration.*/
    int duration_total = 0;
    /** Track current state.*/
    TrackState trackState = STOPPED;

    public:

    /** Constructor.
     * 
     *  @param player Audio player.
     * 
     *  @param name Player name.
     * 
     *  @param screeanGeometry Geometry of the computer's primary screen.
    */
    MediaFilesPlayerManager(QTabWidget *devices, QString name, QRect *screeanGeometry, QWidget *parent = nullptr);

    private:

    /** Handles entering drag event.*/
    void dragEnterEvent(QDragEnterEvent *event);
    /** Handles items (with appropriate type) dropped on this widget.*/
    void dropEvent(QDropEvent *event);

    /** Display player error.
     * 
     *  @param message error message.
    */
    void playerError(QString message);

    /** Display player error.
     * 
     *  @param value volume value. int[0..100] is converted to float[0..1]
    */
    void setVolume(int value);
    /** Updates track current duration.*/
    void updateDuration();

    /** Stop audio track.*/
    void trackStop();
    /** Audio track play-pause.*/
    void trackPlayPause();
    /** React to player sending signalTrackEnd(). Update track state to STOPPED.*/
    void playerTrackEnded();

    Q_OBJECT
    signals:
    /** Ask player to set new track.
     * 
     *  @param filepath media file path.
    */
    void askNewTrack(QString filepath);
    /** Ask player to change track state.
     * 
     *  @param state new track state.
    */
    void askNewTrackState(TrackState state);
};