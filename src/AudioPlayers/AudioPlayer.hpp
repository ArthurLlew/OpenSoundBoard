#pragma once

// Qt
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QRunnable>
#include <QtWidgets/QWidget>
#include <QtWidgets/QTabWidget>
// QtMultimedia
#include <QtMultimedia/QAudioSink>
// Custom
#include <DeviceTab.hpp>
#include <AudioPlayers/AudioTrackContext.hpp>


/** Basic player class.*/
class AudioPlayer : public QObject, public QRunnable
{
    // Mandatory for QWidget stuff to work
    Q_OBJECT

protected:

    /** Tab widget that describes available devices.*/
    QTabWidget const *devices = nullptr;

    /** Device update schedule.*/
    bool mustUpdateDevices = false;

    /** Audio output (virtual cable).*/
    QAudioSink *audioVCableSink = nullptr;
    QIODevice *audioVCableSinkIO = nullptr;
    /** Audio output.*/
    QAudioSink *audioSink = nullptr;
    QIODevice *audioSinkIO = nullptr;

public:

    /** Constructor.
     * 
     *  @param devices Tab widget that describes avaliavle devices.
    */
    explicit AudioPlayer(QTabWidget const *devices);

protected:

    /** Restarts given audio sink.
     * 
     *  @param audioSink Audio sink to restart.
     *  @param deviceTab Device tab with audio device info.
     *  @param format Audio format to use when opening audio sink.
     * 
     *  @return IO device of restarted audio sink.
    */
    virtual QIODevice* restartAudioSink(QAudioSink **audioSink, DeviceTab *deviceTab, const QAudioFormat &format);

public:

    /** Updates audio devices.*/
    virtual void updateAudioDevices();

    /** Player cycle (pure virtual).*/
    virtual void run() = 0;

signals:
    /** Emitted when player encounters any error.
     * 
     *  @param message error message.
    */
    void signalError(QString message);
};