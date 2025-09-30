#include <AudioPlayers/AudioPlayer.hpp>


AudioPlayer::AudioPlayer(QTabWidget const* devices)
{
    this->devices = devices;

    // Do not allow thread pool to destroy us
    setAutoDelete(false);
}


void AudioPlayer::updateAudioDevices()
{
    mustUpdateDevices = true;
}


QIODevice* AudioPlayer::restartAudioSink(QAudioSink **audioSink, DeviceTab *deviceTab, const QAudioFormat &format)
{
    // Stop and delete previous audio sink
    if (*audioSink != nullptr)
    {
        (*audioSink)->stop();
        delete *audioSink;
    }

    // Get currrently selected device
    QAudioDevice audio_device = deviceTab->getDevice();

    // Check if any device is avaliable
    if (audio_device.isNull())
        throw std::runtime_error("Audio output: No devices avaliable");

    // Check if audio format is supported
    if (!audio_device.isFormatSupported(format))
        throw std::runtime_error("Audio output: Audio format is not supported");

    // Init and start audio sink
    *audioSink = new QAudioSink(audio_device, format);
    return (*audioSink)->start();
}