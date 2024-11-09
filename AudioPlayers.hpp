#pragma once

// Qt5
#include <QtCore/QObject>
#include <QtCore/QRunnable>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QAbstractItemView>
// PortAudio
#include <portaudio.h>
// Mutex for handling threads
#include <mutex>
// Custom
#include "DeviceTab.hpp"
#include "AudioTrack.hpp"

using namespace std;


// Basic player class
class AudioPlayer : public QObject, public QRunnable
{
    Q_OBJECT

    protected:
    // Lists of devices
    QTabWidget *devices;

    public:

    // Tells the player if he should continue with the cycle
    bool is_alive = false;

    // Constructor
    AudioPlayer(QTabWidget *devices);

    // Stops the player if it is running
    void kill();
    // Opens stream for the provided device
    PaStream* open_device_stream(DeviceTab* device_tab, int sample_rate, PaSampleFormat sampleFormat);

    // Player cycle (pure virtual)
    virtual void run() = 0;

    signals:
    // Signals that the player has stopped working
    void cant_open_stream();
};


// Player that reroutes microphone input to outputs
class MicrophonePlayer : public AudioPlayer
{
    public:

    // Constructor
    MicrophonePlayer(QTabWidget *devices);

    // Player cycle
    void run();
};


// Player that manages media files
class  MediaFilesPlayer : public AudioPlayer
{
    // List of tracks (with mutex)
    QListWidget *tracks;

    public:

    // Constructor
    MediaFilesPlayer(QTabWidget *devices, QListWidget *tracks);

    // Player cycle
    void run();
};