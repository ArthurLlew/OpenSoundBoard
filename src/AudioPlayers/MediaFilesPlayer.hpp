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
     * Describes player state.
     */
    enum State{
        STOPPED,
        PLAYING,
        PAUSED
    };

private:

    // Current track.
    AudioTrackContext *track = nullptr;
    // Current state.
    State state = STOPPED;
    // Requested state.
    State scheduledState = STOPPED;

    // Whether player should read next samples
    bool shouldReadSamples = true;

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

private:
    /**
     * @param state new player state
     */
    void setState(State state);
public:

    /**
     * Player cycle.
     */
    void run();

    /**
     * Sets audio track.
     */
    void setTrack(QString filepath);

private:
    /**
     * Flushes current audio track info.
     */
    void removeTrack();
public:

    /**
     * Sets audio track volume.
     */
    void setVolume(qreal volume);

    /**
     * @param state planned player state
     */
    void scheduleState(State state);
    
signals:
    /**
     * Signals to update player state.
     */
    void signalState(State state);
    /**
     * Signals to update duration of time slider.
     */
    void signalDuration(double seconds);
    /**
     * Signals to update time slider.
     */
    void signalTime(double seconds);
};