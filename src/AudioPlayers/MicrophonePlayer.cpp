#include <AudioPlayers/MicrophonePlayer.hpp>


MicrophonePlayer::MicrophonePlayer(QTabWidget const* devices) : AudioPlayer(devices) {}


void MicrophonePlayer::run()
{
    isRunning = true;

    try
    {
        // Player cycle
        while (isRunning)
        {
            // Update streams if needed
            if (mustUpdateDevices)
            {
                audioSource = restartAudioStream(&audioSource, (DeviceTab*)devices->widget(0));
                audioVCableSink = restartAudioStream(&audioVCableSink, (DeviceTab*)devices->widget(1), audioSource->format());
                mustUpdateDevices = false;
            }

            #define AUDIO_BUFFER_SIZE 1024 

            // Read microphone input
            float buffer[AUDIO_BUFFER_SIZE * SDL_AUDIO_FRAMESIZE(audioSource->format())];
            // Read samples
            if (shouldReadSamples)
            {
                audioSource->read(buffer, AUDIO_BUFFER_SIZE);
                shouldReadSamples = false;
            }

            // Write data if enough space is available
            if (!shouldReadSamples && audioVCableSink->isReadyForWrite(AUDIO_BUFFER_SIZE))
            {
                audioVCableSink->write(buffer, AUDIO_BUFFER_SIZE);
                shouldReadSamples = true;
            }

            #undef AUDIO_BUFFER_SIZE
        }

        // Flush for correct audio ending
        if (shouldFlush)
        {
            audioVCableSink->flush();
            shouldFlush = false;
        }

        // Wait for audio data to end
        while(!audioVCableSink->isEmpty()) {}
    }
    catch(const std::exception& e)
    {
        emit signalError(e.what());
    }

    // Stop streams
    stopAudioStream(&audioSource);
    stopAudioStream(&audioVCableSink);
    
    // Reset player
    reset();
}


void MicrophonePlayer::stop()
{
    isRunning = false;
}