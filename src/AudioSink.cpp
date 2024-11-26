#include "AudioSink.hpp"


AudioSink::AudioSink(DeviceTab *deviceTab, const QAudioFormat &format)
{
    // Get currrently selected device
    QAudioDevice audio_device = deviceTab->getDevice();

    // Check if any device is avaliable
    if (!audio_device.isNull())
    {
        // Check if audio format is supported
        if (audio_device.isFormatSupported(format))
        {
            // Init and start audio sink
            audioSink = new QAudioSink(audio_device, format);
            audioSinkIO = audioSink->start();
        }
        else
        {
            errorMsg = "Audio format is not supported";
        }
    }
    else
    {
        errorMsg = "No devices avaliable";
    }
}


AudioSink::~AudioSink()
{
    if (audioSink)
    {
        // Stop and delete audio sink
        audioSink->stop();
        delete audioSink;
    }
}


QString AudioSink::error() const
{
    // Do not allow the stream to suddenly stop without our direct order
    if (audioSink)
        if (audioSink->state() == QtAudio::StoppedState)
            return "Audio stream suddenly stopped";

    return errorMsg;
}


QAudioFormat AudioSink::format() const
{
    // If any error was encountered in init return empty format
    return audioSink ? audioSink->format() : QAudioFormat();
}


bool AudioSink::bytesFree(qint64 bytes) const
{
    // If any error was encountered in init return true
    return audioSink ? (audioSink->bytesFree() < bytes) : false;
}


void AudioSink::setVolume(qreal volume)
{
    if(audioSink)
        audioSink->setVolume(volume);
}


qint64 AudioSink::write(const QByteArray &data)
{
    // If any error was encountered in init return 0
    return audioSink ? audioSinkIO->write(data) : 0;
}