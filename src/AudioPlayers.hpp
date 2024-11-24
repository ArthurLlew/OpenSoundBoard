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
#include "AudioTrackContext.hpp"


/** Basic player class.*/
class AudioPlayer : public QObject, public QRunnable
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

protected:

    /** tab widget that describes avaliavle devices.*/
    QTabWidget const *devices = nullptr;
    /** Player state.*/
    bool is_alive = false;

public:

    /** Constructor.
     * 
     *  @param devices Tab widget that describes avaliavle devices.
    */
    explicit AudioPlayer(QTabWidget const *devices);

public:

    /** Stops the player if it is running.*/
    void stop();
    /** Player cycle (pure virtual).*/
    virtual void run() = 0;

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
    // Mandatory for QWidget stuff to work
    Q_OBJECT

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
    // Mandatory for QWidget stuff to work
    Q_OBJECT

    /** Current track.*/
    AudioTrackContext *track = nullptr;
    /** Audio output (virtual cable).*/
    QAudioSink *audio_sink_cable = nullptr;
    /** Audio output.*/
    QAudioSink *audio_sink = nullptr;
    /** Audio volume.*/
    float volume = 1.0;
    /** Track current state.*/
    AudioTrackContext::TrackState nextTrackState = AudioTrackContext::STOPPED;

public:

    /** Constructor.
     * 
     *  @param devices Tab widget that describes available devices.
    */
    MediaFilesPlayer(QTabWidget const *devices);
    /** Destructor.*/
    ~MediaFilesPlayer();

    /** Player cycle.*/
    void run();

    /** Sets new track to play.*/
    void setNewTrack(QString filepath);
    /** Sets new track state.*/
    void setNewTrackState(AudioTrackContext::TrackState state);
    /** Sets track volume.*/
    void setNewTrackVolume(float volume);

    signals:
    /** Emitted when player track has ended.*/
    void signalTrackEnd();
};