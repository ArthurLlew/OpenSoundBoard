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
#include <QtGui/QScreen>
#include <QtWidgets/QApplication>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QProgressbar>
// Custom
#include <WidgetMessages.hpp>
#include <AudioPlayerManagers/AudioPlayerManager.hpp>
#include <AudioPlayers/MediaFilesPlayer.hpp>


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
    */
    explicit MediaFilesPlayerManager(QTabWidget const *devices, QString name, QWidget *parent = nullptr);
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