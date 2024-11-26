#pragma once

// Qt
#include <QtCore/QString>
// QtMultimedia
#include <QtMultimedia/QAudioSink>
// Custom
#include "DeviceTab.hpp"


class AudioSink
{
    QString errorMsg;
    /** Audio sink.*/
    QAudioSink *audioSink = nullptr;
    /** Audio sink io device.*/
    QIODevice *audioSinkIO = nullptr;

public:

    /** Constructor.
     * 
     *  @param deviceTab Device tab with audio device info.
     *  @param format Audio format to use when opening audio sink.
    */
    explicit AudioSink(DeviceTab *deviceTab, const QAudioFormat &format = QAudioFormat());
    /** Destructor.*/
    ~AudioSink();

    /** Returns error status of audio sink.
     * 
     *  @return Empty string if audio sink can be used, error string if any error occurred.
    */
    QString error() const;

    /** Returns audio format used in audio sink.
     * 
     *  @return Audio format.
    */
    QAudioFormat format() const;

    /** Tells if requested ammount of bytes is available in audio sink buffer and audio sink is alive.
     * 
     *  @return True if requested space is available and audio sink is active, else false.
    */
    bool bytesFree(qint64 bytes) const;

    /** Changes audio sink volume.
     * 
     *  @param volume New audio sink volume.
    */
    void setVolume(qreal volume);

    /** Writes audio data to audio sink device.
     * 
     *  @param data Audio data to write.
     * 
     *  @return Writen bytes.
    */
    qint64 write(const QByteArray &data);
};