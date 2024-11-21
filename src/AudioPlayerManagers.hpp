#pragma once

// Qt
#include <QtCore/QObject>
#include <QtCore/QMimeData>
#include <QtCore/QString>
#include <QtCore/QThreadPool>
#include <QtCore/QRunnable>
#include <QtGui/QPainter>
#include <QtGui/QDragEnterEvent>
#include <QtGui/QDropEvent>
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
    // Mandatory for QWidget stuff to work
    Q_OBJECT

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
     *  @param name player name.
    */
    explicit AudioPlayerManager(AudioPlayer *player, QString name, QWidget *parent = nullptr);
    /** Destructor.*/
    ~AudioPlayerManager();

protected:

    /** Handles paint event.*/
    void paintEvent(QPaintEvent *) override;

    /** Display player error.
     * 
     *  @param message error message.
    */
    void playerError(QString message);

    /** Start/Stop player.*/
    void playerRunStop();

public:

    /** Returns player state.*/
    bool playerState();
    /** Start player.*/
    void playerRun();
    /** Stop player.*/
    void playerStop();
    /** Wait for the player to stop.*/
    void playerWait();

signals:
    /** Ask player to stop.*/
    void askPlayerStop();
};


/** Microphone player manager.*/
class MicrophonePlayerManager : public AudioPlayerManager
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

public:

    /** Constructor.
     * 
     *  @param player Audio player.
     *  @param name player name.
    */
    MicrophonePlayerManager(QTabWidget *devices, QString name, QWidget *parent = nullptr);
};


/** Media files player manager.*/
class MediaFilesPlayerManager : public AudioPlayerManager
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

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
    /** Track current state.*/
    TrackState trackState = STOPPED;

public:

    /** Constructor.
     * 
     *  @param player Audio player.
     *  @param name Player name.
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

    /** Stop audio track.*/
    void trackStop();
    /** Audio track play-pause.*/
    void trackPlayPause();
    /** React to player sending signalTrackEnd(). Update track state to STOPPED.*/
    void playerTrackEnded();

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