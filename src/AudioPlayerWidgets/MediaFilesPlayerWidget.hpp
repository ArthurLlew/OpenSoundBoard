#pragma once


// Qt widgets
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
// Audio player widget
#include <AudioPlayerWidgets/AudioPlayerWidget.hpp>
// Media files player
#include <AudioPlayers/MediaFilesPlayer.hpp>


#define VOLUME_SLIDER_SCALE 10


/**
 * Media files player widget.
 */
class MediaFilesPlayerWidget : public AudioPlayerWidget
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

    // Track name label.
    QLabel *trackName = nullptr;
    // Play button.
    QPushButton *buttonPlay = nullptr;
    // Volume label.
    QLabel *volumeLabel = nullptr;
    // Time slider.
    QSlider *time_slider = nullptr;
    // Whether player was paused by time slider.
    bool wasPausedByTimeSlider = false;

public:

    /**
     * Constructor.
     * 
     * @param player Audio player.
     * @param name Player name.
     */
    explicit MediaFilesPlayerWidget(QTabWidget const *devices, QString name, QWidget *parent = nullptr);
    /**
     * Destructor.
     */
    ~MediaFilesPlayerWidget();

private:

    /**
     * Handles entering drag event.
     */
    void dragEnterEvent(QDragEnterEvent *event);
    /**
     * Handles items (with appropriate type) dropped on this widget.
     */
    void dropEvent(QDropEvent *event);

    /**
     * Sets player volume.
     * 
     * @param value volume value. int[0..100] is converted to float[0..1]
     */
    void setVolume(int value);

    /**
     * Sets player time.
     * 
     * @param value slider position in its ticks
     */
    void setTime(int value);

    /**
     * Pauses player when timestamp is changed by user.
     */
    void pauseOnTimeChange();

    /**
     * Resume player after timestamp was changed by user.
     */
    void resumeAfterTimeChange();

    /**
     * Start/Stop player.
     */
    void startStop() override;

public:

    /**
     * Stops player.
     */
    void stop() override;
    
    /**
     * Handler for player state update signal.
     */
    void onStateChanged(MediaFilesPlayer::State state);
    
    /**
     * Handler for player duration update signal.
     * 
     * @param seconds duration in seconds
     */
    void onDurationChanged(double seconds);
    
    /**
     * Handler for player time update signal.
     * 
     * @param seconds time in seconds
     */
    void onTimeChanged(double seconds);

signals:
    /**
     * Ask player to change track.
     * 
     * @param filepath media file path
     */
    void askNewTrack(QString filepath);
    /**
     * Ask player to change state.
     * 
     * @param state new state
     */
    void askNewState(MediaFilesPlayer::State state);
    /**
     * Ask player to change volume.
     * 
     * @param volume volume value
     */
    void askNewVolume(float volume);
    /**
     * Ask player to change time.
     * 
     * @param seconds time in seconds
     */
    void askNewTime(double seconds);
};