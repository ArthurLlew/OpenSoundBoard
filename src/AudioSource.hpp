#pragma once

// Qt
#include <QtCore/QString>
// QtMultimedia
#include <QtMultimedia/QAudioSource>
// Custom
#include "DeviceTab.hpp"


class AudioSource
{
    QString errorMsg;
    /** Audio sink.*/
    QAudioSource *audioSource = nullptr;
    /** Audio sink io device.*/
    QIODevice *audioSourceIO = nullptr;

public:

    /** Constructor.
     * 
     *  @param deviceTab Device tab with audio device info.
    */
    explicit AudioSource(DeviceTab *deviceTab);
    /** Destructor.*/
    ~AudioSource();

    /** Returns error status of audio source.
     * 
     *  @return Empty string if audio sink can be used, error string if any error occurred.
    */
    QString error() const;

    /** Returns audio format used in audio source.
     * 
     *  @return Audio format.
    */
    QAudioFormat format() const;

    /** Reads audio data from audio source device.
     * 
     *  @param maxlen Maximum for the amount of bytes to read.
     * 
     *  @return Read data.
    */
    QByteArray read(qint64 maxlen);
};