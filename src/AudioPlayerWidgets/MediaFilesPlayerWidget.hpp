#pragma once


// Qt widgets
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QPushButton>
// Audio player widget
#include <AudioPlayerWidgets/AudioPlayerWidget.hpp>
// Media files player
#include <AudioPlayers/MediaFilesPlayer.hpp>


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
     * Display player error.
     * 
     * @param value volume value. int[0..100] is converted to float[0..1]
     */
    void setVolume(int value);

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
     * Handler for track state update signal.
     */
    void onTrackStateChanged(MediaFilesPlayer::State state);

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
    /** Ask player to change volume.
     * 
     *  @param volume volume value
     */
    void askNewVolume(qreal volume);
};