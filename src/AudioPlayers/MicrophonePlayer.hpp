#pragma once


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
    DeviceStream *audioSource = nullptr;

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

    /**
     * Player cycle.
     */
    void run() override;
    /**
     * Stops the player if it is running.
     */
    void stop();
};