#pragma once

// Qt
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QThreadPool>
#include <QtCore/QRunnable>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QAbstractItemView>
// QtMultimedia
#include <QtMultimedia/QAudioSource>
#include <QtMultimedia/QAudioSink>
// Custom
#include "DeviceTab.hpp"
#include "AudioSink.hpp"
#include "AudioSource.hpp"
#include "AudioTrackContext.hpp"


/** Basic player class.*/
class AudioPlayer : public QObject, public QRunnable
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

protected:

    /** Device update schedule.*/
    bool mustUpdateDevices = false;

    /** Tab widget that describes available devices.*/
    QTabWidget const *devices = nullptr;

public:

    /** Constructor.
     * 
     *  @param devices Tab widget that describes avaliavle devices.
    */
    explicit AudioPlayer(QTabWidget const *devices);

    /** Player cycle (pure virtual).*/
    virtual void run() = 0;

    /** Updates audio streams.*/
    virtual void updateAudioStreams();

signals:
    /** Emitted when player encounters any error.
     * 
     *  @param message error message.
    */
    void signalError(QString message);
};


/** Player that reroutes microphone input to outputs.*/
class MicrophonePlayer : public AudioPlayer
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

    /** Player state.*/
    bool isRunning = false;

    /** Audio input.*/
    AudioSource *audioSource = nullptr;
    /** Audio output (virtual cable).*/
    AudioSink *audioVCableSink = nullptr;
    /** Audio output.*/
    AudioSink *audioSink = nullptr;

public:

    /** Constructor.
     * 
     *  @param devices Tab widget that describes avaliavle devices.
    */
    explicit MicrophonePlayer(QTabWidget const *devices);

    /** Player cycle.*/
    void run();
    /** Stops the player if it is running.*/
    void stop();
};


/** Player that manages media files.*/
class  MediaFilesPlayer : public AudioPlayer
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

    /** Current track.*/
    AudioTrackContext *track = nullptr;
    /** Requested track state.*/
    AudioTrackContext::TrackState newTrackState = AudioTrackContext::STOPPED;
    /** Audio volume.*/
    float volume = 0.5;

    /** Audio output (virtual cable).*/
    AudioSink *audioVCableSink = nullptr;
    /** Audio output.*/
    AudioSink *audioSink = nullptr;

public:

    /** Constructor.
     * 
     *  @param devices Tab widget that describes available devices.
    */
    explicit MediaFilesPlayer(QTabWidget const *devices);
    /** Destructor.*/
    ~MediaFilesPlayer();

    /** Player cycle.*/
    void run();

    /** Sets new audio track to play.*/
    void setNewTrack(QString filepath);
    /** Sets new audio track state.*/
    void setNewTrackState(AudioTrackContext::TrackState state);
    /** Sets audio track volume.*/
    void setNewTrackVolume(float volume);

signals:
    /** Emitted when audio track has ended.*/
    void signalTrackEnd();
};