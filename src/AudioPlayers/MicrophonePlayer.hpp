#pragma once


// QtMultimedia
#include <QtMultimedia/QAudioSource>
// Audio player
#include <AudioPlayers/AudioPlayer.hpp>


/**
 * Player that reroutes microphone input to selected output.
 */
class MicrophonePlayer : public AudioPlayer
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

    // Player state.
    bool isRunning = false;

    // Audio input.
    QAudioSource *audioSource = nullptr;
    // Audio input IO.
    QIODevice *audioSourceIO = nullptr;

public:

    /**
     * Constructor.
     * 
     * @param devices tab widget that describes avaliavle devices
     */
    explicit MicrophonePlayer(QTabWidget const *devices);

    /**
     * @return player state
     */
    bool getState() { return isRunning; }

private:

    /**
     * Restarts given audio source.
     * 
     * @param audioSink audio source to restart
     * @param deviceTab device tab with audio device info
     * 
     * @return IO device of restarted audio source
     */
    virtual QIODevice* restartAudioSource(QAudioSource **audioSource, DeviceTab *deviceTab);

public:

    /**
     * Player cycle.
     */
    void run() override;
    /**
     * Stops the player if it is running.
     */
    void stop();
};