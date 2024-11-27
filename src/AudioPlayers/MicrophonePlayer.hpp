#pragma once

// Qt
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QRunnable>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTabWidget>
// QtMultimedia
#include <QtMultimedia/QAudioSource>
// Custom
#include <DeviceTab.hpp>
#include <AudioPlayers/AudioPlayer.hpp>
#include <AudioPlayers/AudioTrackContext.hpp>


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