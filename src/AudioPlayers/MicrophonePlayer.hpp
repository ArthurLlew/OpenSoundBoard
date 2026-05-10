#pragma once


// QtMultimedia
#include <QtMultimedia/QAudioSource>
// Audio player
#include <AudioPlayers/AudioPlayer.hpp>


/** Player that reroutes microphone input to outputs.*/
class MicrophonePlayer : public AudioPlayer
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

    /** Player state.*/
    bool isRunning = false;

    /** Audio input.*/
    QAudioSource *audioSource = nullptr;
    QIODevice *audioSourceIO = nullptr;

public:

    /** Constructor.
     * 
     *  @param devices Tab widget that describes avaliavle devices.
    */
    explicit MicrophonePlayer(QTabWidget const *devices);

private:

    /** Restarts given audio source.
     * 
     *  @param audioSink Audio source to restart.
     *  @param deviceTab Device tab with audio device info.
     * 
     *  @return IO device of restarted audio source.
    */
    virtual QIODevice* restartAudioSource(QAudioSource **audioSource, DeviceTab *deviceTab);

public:

    /** Player cycle.*/
    void run();
    /** Stops the player if it is running.*/
    void stop();
};