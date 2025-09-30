#include <AudioPlayers/MicrophonePlayer.hpp>


MicrophonePlayer::MicrophonePlayer(QTabWidget const* devices) : AudioPlayer(devices) {}


QIODevice* MicrophonePlayer::restartAudioSource(QAudioSource **audioSource, DeviceTab *deviceTab)
{
    // Stop and delete previous audio source
    if (*audioSource != nullptr)
    {
        (*audioSource)->stop();
        delete *audioSource;
    }

    // Get currrently selected device
    QAudioDevice audio_device = deviceTab->getDevice();

    // Check if any device is avaliable
    if (audio_device.isNull())
        throw std::runtime_error("Audio input: No devices avaliable");

    // Get and modify preffered format
    QAudioFormat format = audio_device.preferredFormat();
    format.setChannelCount(2);

    // Init and start audio source
    *audioSource = new QAudioSource(audio_device, format);
    return (*audioSource)->start();
}


void MicrophonePlayer::run()
{
    isRunning = true;
    // Create audio streams on launch
    mustUpdateDevices = true;

    try
    {
        // Player cycle
        while (isRunning)
        {
            // Update streams if needed
            if (mustUpdateDevices)
            {
                audioSourceIO = restartAudioSource(&audioSource, (DeviceTab*)devices->widget(0));
                audioVCableSinkIO = restartAudioSink(&audioVCableSink, (DeviceTab*)devices->widget(1), audioSource->format());
                mustUpdateDevices = false;
            }

            // Check streams
            if (audioSource->state() == QtAudio::StoppedState)
                throw std::runtime_error("Audio input suddenly stopped");
            if (audioVCableSink->state() == QtAudio::StoppedState)
                throw std::runtime_error("Audio Virtual cable output suddenly stopped");

            #define AUDIO_BYTE_SIZE 1024 

            // Read microphone input
            QByteArray audio = audioSourceIO->read(AUDIO_BYTE_SIZE);

            // Wait for availiable space and write data
            while (audioVCableSink->bytesFree() < AUDIO_BYTE_SIZE) {}
            audioVCableSinkIO->write(audio);

            #undef AUDIO_BYTE_SIZE
        }
    }
    catch(const std::exception& e)
    {
        emit signalError(e.what());
    }

    // Free streams
    audioSource->stop();
    delete audioSource;
    audioSource = nullptr;
    audioVCableSink->stop();
    delete audioVCableSink;
    audioVCableSink = nullptr;
}


void MicrophonePlayer::stop()
{
    isRunning = false;
}