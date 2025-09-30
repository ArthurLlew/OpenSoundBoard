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
#include <AudioPlayerWidgets/AudioPlayerWidget.hpp>
#include <AudioPlayers/MediaFilesPlayer.hpp>


/** Media files player widget.*/
class MediaFilesPlayerWidget : public AudioPlayerWidget
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
    explicit MediaFilesPlayerWidget(QTabWidget const *devices, QString name, QWidget *parent = nullptr);
    /** Destructor.*/
    ~MediaFilesPlayerWidget();

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

    /** Start/Stop player.*/
    void startStop();

public:

    /** Stops player.*/
    void stop();

    /** Updates devices in running player.*/
    void updateDevices();
    
    /** Handler for track state update signal.*/
    void onTrackStateChanged(AudioTrackContext::TrackState state);

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
    void askNewTrackVolume(qreal volume);
};