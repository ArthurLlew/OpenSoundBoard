#pragma once

// Qt5
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QThreadPool>
#include <QtCore/QRunnable>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QAbstractItemView>
// PortAudio
#include <portaudio.h>
// Custom
#include "DeviceTab.hpp"
#include "AudioTrackContext.hpp"

using namespace std;


/** Basic player class.*/
class AudioPlayer : public QObject, public QRunnable
{
    protected:

    /** tab widget that describes avaliavle devices.*/
    QTabWidget const *devices;
    /** Player state.*/
    bool is_alive = false;

    public:

    /** Constructor.
     * 
     *  @param devices Tab widget that describes avaliavle devices.
    */
    AudioPlayer(QTabWidget const *devices);

    protected:

    /** Opens stream for the provided device.
     * 
     *  @param targetDevice Device for which we are openning stream.
     * 
     *  @param sampleFormat Stream sample format.
     * 
     *  @param SampleRate Stream sample rate.
     * 
     *  @param sourceDevice Device where data from this stream will go (affects stream settings).
     * 
     *  @return Opened and active stream.
    */
    PaStream* openDeviceStream(DeviceTab const *targetDevice, PaSampleFormat sampleFormat, double SampleRate = 0,
                                 DeviceTab const *sourceDevice = nullptr);

    public:

    /** Stops the player if it is running.*/
    void stop();
    /** Player cycle (pure virtual).*/
    virtual void run() = 0;

    Q_OBJECT
    signals:
    /** Emitted when the player encounters any error.
     * 
     *  @param message error message.
    */
    void signalError(QString message);
};


/** Player that reroutes microphone input to outputs.*/
class MicrophonePlayer : public AudioPlayer
{
    public:

    /** Constructor.
     * 
     *  @param devices Tab widget that describes avaliavle devices.
    */
    MicrophonePlayer(QTabWidget const *devices);

    /** Player cycle.*/
    void run();
};


/** Player that manages media files.*/
class  MediaFilesPlayer : public AudioPlayer
{
    /** Current track.*/
    AudioTrackContext *track = nullptr;
    /** Pointer to volume.*/
    float const *volume;
    /** Track current state.*/
    TrackState nextTrackState = STOPPED;

    public:

    /** Constructor.
     * 
     *  @param devices Tab widget that describes avaliavle devices.
     * 
     *  @param volume Pointer to volume.
    */
    MediaFilesPlayer(QTabWidget const *devices, float const *volume);
    /** Destructor.*/
    ~MediaFilesPlayer();

    /** Player cycle.*/
    void run();

    /** Sets new track to play.*/
    void setNewTrack(QString filepath);
    /** Sets new track state.*/
    void setNewTrackState(TrackState state);

    Q_OBJECT
    signals:
    /** Emitted when player track has ended.*/
    void signalTrackEnd();
};