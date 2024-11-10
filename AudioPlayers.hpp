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


// Basic player class
class AudioPlayer : public QObject, public QRunnable
{
    Q_OBJECT

    protected:

    // Lists of devices
    QTabWidget const *devices;

    public:

    // Tells the player if he should continue with the cycle
    bool is_alive = false;

    // Constructor
    AudioPlayer(QTabWidget const *devices);

    protected:

    // Opens stream for the provided device
    PaStream* open_device_stream(DeviceTab const *device_tab, int sample_rate, PaSampleFormat sampleFormat);

    public:

    // Stops the player if it is running
    void kill();
    // Player cycle (pure virtual)
    virtual void run() = 0;

    signals:
    // Emitted when the player encounters any error
    void player_error(QString message);
};


// Player that reroutes microphone input to outputs
class MicrophonePlayer : public AudioPlayer
{
    public:

    // Constructor
    MicrophonePlayer(QTabWidget const *devices);

    // Player cycle
    void run();
};


// Player that manages media files
class  MediaFilesPlayer : public AudioPlayer
{
    Q_OBJECT

    // Current track
    AudioTrackContext *track = nullptr;
    // Stores where to grab volume value
    float const *volume_ptr;
    // Tells track current state
    TrackState next_track_state = STOPPED;

    public:

    // Constructor
    MediaFilesPlayer(QTabWidget const *devices, float const *volume_ptr);
    // Destructor
    ~MediaFilesPlayer();

    // Player cycle
    void run();

    // Sets new track to play
    void new_track(QString filepath);
    // Sets new track state
    void new_track_state(TrackState track_state);

    signals:
    // Emitted when player track has ended
    void track_endeded();
};