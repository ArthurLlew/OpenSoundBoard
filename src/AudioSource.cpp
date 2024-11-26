#include "AudioSource.hpp"


AudioSource::AudioSource(DeviceTab *deviceTab)
{
    // Get currrently selected device
    QAudioDevice audio_device = deviceTab->getDevice();

    // Check if any device is avaliable
    if (!audio_device.isNull())
    {
        // Get and modify preffered format
        QAudioFormat format = audio_device.preferredFormat();
        format.setChannelCount(2);

        // Check if audio format is supported
        if (audio_device.isFormatSupported(format))
        {
            // Init and start audio sink
            audioSource = new QAudioSource(audio_device, format);
            audioSourceIO = audioSource->start();
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


AudioSource::~AudioSource()
{
    if (audioSource)
    {
        // Stop and delete audio source
        audioSource->stop();
        delete audioSource;
    }
}


QString AudioSource::error() const
{
    // Do not allow the stream to suddenly stop without our direct order
    if (audioSource)
        if (audioSource->state() == QtAudio::StoppedState)
            return "Audio stream suddenly stopped";

    return errorMsg;
}


QAudioFormat AudioSource::format() const
{
    // If any error was encountered in init return empty format
    return audioSource ? audioSource->format() : QAudioFormat();
}


QByteArray AudioSource::read(qint64 maxlen)
{
    // If any error was encountered in init return empty array
    return audioSource ? audioSourceIO->read(maxlen) : QByteArray();
}