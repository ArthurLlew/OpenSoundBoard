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


void AudioPlayer::stopAudioStream(DeviceStream **audioStream)
{
    // Stop and delete previous audio stream
    if (*audioStream)
    {
        delete *audioStream;
        *audioStream = nullptr;
    }
}


DeviceStream* AudioPlayer::restartAudioStream(DeviceStream **audioSink, DeviceTab *deviceTab, const SDL_AudioSpec &format)
{
    // Stop audio sink
    stopAudioStream(audioSink);

    // Init and start new audio
    return new DeviceStream(deviceTab->getDevice(), format);
}


DeviceStream* AudioPlayer::restartAudioStream(DeviceStream **audioSink, DeviceTab *deviceTab)
{
    // Stop audio sink
    stopAudioStream(audioSink);

    // Init and start new audio
    return new DeviceStream(deviceTab->getDevice());
}


void AudioPlayer::reset()
{
    // Raise update flags
    mustUpdateDevices = true;
    shouldReadSamples = true;
    shouldFlush = true;
}