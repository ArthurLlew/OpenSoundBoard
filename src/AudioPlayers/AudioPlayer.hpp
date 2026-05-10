#pragma once


// Qt core
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QRunnable>
// Qt widgets
#include <QtWidgets/QWidget>
#include <QtWidgets/QTabWidget>
// QtMultimedia
#include <QtMultimedia/QAudioSink>
// Device tab widget
#include <DeviceTab.hpp>


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

    // Whether devices should be updated (always update at startup).
    bool mustUpdateDevices = true;

    // Audio output (virtual cable).
    QAudioSink *audioVCableSink = nullptr;
    // Audio output IO (virtual cable).
    QIODevice *audioVCableSinkIO = nullptr;
    // Audio output.
    QAudioSink *audioSink = nullptr;
    // Audio output IO.
    QIODevice *audioSinkIO = nullptr;

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
    template<typename QAudioStream>
    void stopAudioStream(QAudioStream **audioStream)
    {
        // Stop and delete previous audio stream
        if (*audioStream != nullptr)
        {
            (*audioStream)->stop();
            delete *audioStream;
            *audioStream = nullptr;
        }
    }

    /**
     * Restarts given audio sink.
     * 
     * @param audioSink Audio sink to restart
     * @param deviceTab Device tab with audio device info
     * @param format Audio format to use when opening audio sink
     * 
     * @return IO device of restarted audio sink
     */
    virtual QIODevice* restartAudioSink(QAudioSink **audioSink, DeviceTab *deviceTab, const QAudioFormat &format);

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