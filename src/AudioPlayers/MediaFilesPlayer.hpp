#pragma once


// Audio player
#include <AudioPlayers/AudioPlayer.hpp>
// FFMPEG media files reader
#include <AudioPlayers/AudioTrackReader.cpp>


/**
 * Player that manages media files.
 */
class  MediaFilesPlayer : public AudioPlayer
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

public:

    /**
     * Describes track state.
     */
    enum State{
        STOPPED,
        PLAYING,
        PAUSED
    };

private:

    // Current track.
    AudioTrackContext *track = nullptr;
    // Current track state.
    State state = STOPPED;
    // Requested track state.
    State nextTrackState = STOPPED;

    // Audio volume.
    float volume = 0.5;

public:

    /**
     * Constructor.
     * 
     * @param devices tab widget that describes available devices
     */
    explicit MediaFilesPlayer(QTabWidget const *devices);
    /**
     * Destructor.
     */
    ~MediaFilesPlayer();

    /**
     * @return player state
     */
    State getState() { return track == nullptr ? STOPPED : state; }

    /**
     * Player cycle.
     */
    void run();

    /**
     * Sets audio track.
     */
    void setTrack(QString filepath);

    /**
     * Sets audio track volume
     */
    void setVolume(qreal volume);

private:
    /**
     * @param state new track state
     */
    void setState(State state);
public:
    /**
     * @param state planned track state
     */
    void setPlannedState(State state);
    
signals:
    /**
     * Signals to update audio track state
     */
    void signalState(State state);
};