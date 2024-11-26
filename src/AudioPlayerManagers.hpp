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


/** Audio player manager widget. Can run, stop and wait provided audio player.*/
class AudioPlayerManager : public QWidget, WidgetWarnings
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

protected:

    /** Manager name.*/
    QString name;
    /** Widget main layout.*/
    QVBoxLayout *layout = nullptr;

    /** Thread pool where player will run.*/
    QThreadPool *threadpool = nullptr;
    /** Audio player.*/
    AudioPlayer *player = nullptr;

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
    virtual void playerStartStop() = 0;

public:

    /** Stop player.*/
    virtual void playerStop() = 0;

    /** Updates devices in running player.*/
    virtual void updateDevices() = 0;

signals:
    /** Ask player to update devices.*/
    void askToUpdateDevices();
};


/** Microphone player manager.*/
class MicrophonePlayerManager : public AudioPlayerManager
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

    /** Audio player state.*/
    bool isRunning = false;

    /** Start/Stop button.*/
    QPushButton *buttonStartStop = nullptr;

public:

    /** Constructor.
     * 
     *  @param player Audio player.
     *  @param name player name.
    */
    explicit MicrophonePlayerManager(QTabWidget const *devices, QString name, QWidget *parent = nullptr);
    /** Destructor.*/
    ~MicrophonePlayerManager();

private:

    /** Start/Stop player.*/
    void playerStartStop();

public:

    /** Stop player.*/
    void playerStop();

    /** Updates devices in running player.*/
    void updateDevices();

signals:
    /** Ask player to stop.*/
    void askToStop();
};


/** Media files player manager.*/
class MediaFilesPlayerManager : public AudioPlayerManager
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

    /** Track name label.*/
    QLabel *trackName = nullptr;
    /** Play button.*/
    QPushButton *buttonPlay = nullptr;
    /** Volume label.*/
    QLabel *volumeLabel = nullptr;

    /** Track current state.*/
    AudioTrackContext::TrackState trackState = AudioTrackContext::STOPPED;

public:

    /** Constructor.
     * 
     *  @param player Audio player.
     *  @param name Player name.
     *  @param screeanGeometry Geometry of the computer's primary screen.
    */
    explicit MediaFilesPlayerManager(QTabWidget const *devices, QString name, QRect *screeanGeometry, QWidget *parent = nullptr);
    /** Destructor.*/
    ~MediaFilesPlayerManager();

private:

    /** Handles entering drag event.*/
    void dragEnterEvent(QDragEnterEvent *event);
    /** Handles items (with appropriate type) dropped on this widget.*/
    void dropEvent(QDropEvent *event);

    /** Display player error.
     * 
     *  @param value volume value. int[0..100] is converted to float[0..1]
    */
    void setVolume(int value);

    /** Handler for track end signal.*/
    void playerTrackEnded();

    /** Start/Stop player.*/
    void playerStartStop();

public:

    /** Stop player.*/
    void playerStop();

    /** Updates devices in running player.*/
    void updateDevices();

signals:
    /** Ask player to set new track.
     * 
     *  @param filepath Media file path.
    */
    void askNewTrack(QString filepath);
    /** Ask player to change track state.
     * 
     *  @param state New track state.
    */
    void askNewTrackState(AudioTrackContext::TrackState state);
    /** Ask player to change track volume.
     * 
     *  @param volume New track volume.
    */
    void askNewTrackVolume(float volume);
};