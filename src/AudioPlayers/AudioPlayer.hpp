#pragma once


// Qt core
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QRunnable>
// Qt widgets
#include <QtWidgets/QWidget>
#include <QtWidgets/QTabWidget>
// Audio device tab widget
#include <DeviceTab.hpp>
// Audio device stream
#include <SDL/DeviceStream.cpp>


/**
 * Basic player class.
 */
class AudioPlayer : public QObject, public QRunnable
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

protected:

    // Tab widget that describes available devices.
    QTabWidget const *devices = nullptr;

    // Audio output (virtual cable).
    DeviceStream *audioVCableSink = nullptr;

    // Whether devices should be updated (always update at startup).
    bool mustUpdateDevices = true;
    // Whether player should read next samples (always read on startup)
    bool shouldReadSamples = true;
    // Whether stream has to be flushed for correct track ending
    bool shouldFlush = true;

public:

    /**
     * Constructor.
     * 
     * @param devices tab widget that describes avaliavle devices
     */
    explicit AudioPlayer(QTabWidget const *devices);

protected:

    /**
     * Stops given audio stream.
     */
    void stopAudioStream(DeviceStream **audioStream);

    /**
     * Restarts given audio stream.
     * 
     * @param audioSink audio stream to close
     * @param deviceTab device tab with audio device info
     * @param format audio format
     * 
     * @return new audio stream
     */
    DeviceStream* restartAudioStream(DeviceStream **audioSink, DeviceTab *deviceTab, const SDL_AudioSpec &format);

    /**
     * Restarts given audio stream with native format.
     * 
     * @param audioSink audio stream to close
     * @param deviceTab device tab with audio device info
     * 
     * @return new audio stream
     */
    DeviceStream* restartAudioStream(DeviceStream **audioSink, DeviceTab *deviceTab);

    /**
     * Resets player variables.
     */
    void reset();

public:

    /**
     * Updates audio devices.
     */
    virtual void updateAudioDevices();

    /**
     * Player cycle.
     */
    virtual void run() = 0;

signals:
    /**
     * Singals about player error.
     * 
     * @param message error message
     */
    void signalError(QString message);
};