#include <AudioPlayers/MediaFilesPlayer.hpp>


MediaFilesPlayer::MediaFilesPlayer(QTabWidget const *devices) : AudioPlayer(devices) {}


MediaFilesPlayer::~MediaFilesPlayer()
{
    delete track;
}


void MediaFilesPlayer::setState(State state)
{
    if (track)
    {
        // Close track if set to stop
        if (((this->state == PLAYING) || (this->state == PAUSED)) && (state == STOPPED))
        {
            track->close();
        }
        // Start track if was stopped
        if ((this->state == STOPPED) && ((state == PLAYING) || (state == PAUSED)))
        {
            track->open();
        }

        // Update state
        this->state = state;
        scheduledState = state;

        // Notify
        emit signalState(this->state);
    }
}


void MediaFilesPlayer::run()
{
    // No track ==>> no playing
    if (track == nullptr)
        return;

    try
    {
        // Start playing track
        setState(PLAYING);

        // Audio stream format
        SDL_AudioSpec format;
        format.format = SDL_AUDIO_F32;
        format.channels = track->getChannelCount();
        format.freq = track->getSampleRate();

        // Player cycle
        while (state != STOPPED)
        {
            // Update audio streams if needed
            if (mustUpdateDevices)
            {
                audioVCableSink = restartAudioStream(&audioVCableSink, (DeviceTab*)devices->widget(1), format);
                audioVCableSink->volume(volume);
                audioSink = restartAudioStream(&audioSink, (DeviceTab*)devices->widget(2), format);
                audioSink->volume(volume);
                mustUpdateDevices = false;
            }

            // Set scheduled state
            if (state != scheduledState)
            {
                setState(scheduledState);
            }

            // Set scheduled timestamp
            if (scheduledTime >= 0)
            {
                track->setTime(scheduledTime);
                scheduledTime = -1;
                shouldReadSamples = true;
            }

            if (state == PLAYING)
            {
                // Read samples
                if (shouldReadSamples)
                {
                    track->read();
                    emit signalTime(track->getTime());
                    shouldReadSamples = false;
                }

                // If sample count is positive
                if (track->getAudioDataSamplesCount() > 0)
                {
                    // Write data if enough space is available
                    if (audioVCableSink->isReadyForWrite(track->getAudioDataSamplesCount())
                        && audioSink->isReadyForWrite(track->getAudioDataSamplesCount()))
                    {
                        audioVCableSink->write(track->getAudioData()[0], track->getAudioDataSamplesCount());
                        audioSink->write(track->getAudioData()[0], track->getAudioDataSamplesCount());
                        shouldReadSamples = true;
                    }
                }
                else
                {
                    // Flush for correct audio ending
                    if (shouldFlush)
                    {
                        audioVCableSink->flush();
                        audioSink->flush();
                        shouldFlush = false;
                    }

                    // If all previous data was consumed by all streams
                    if (audioVCableSink->isEmpty() && audioSink->isEmpty())
                    {
                        // Set state to stopped
                        setState(STOPPED);
                    }
                }
            }
        }
    }
    catch(const std::exception& e)
    {
        // Error: update track state and notify
        setState(STOPPED);
        emit signalError(e.what());
    }

    // Update track timestamp
    emit signalTime(0);

    // Stop streams
    stopAudioStream(&audioVCableSink);
    stopAudioStream(&audioSink);
    
    // Reset player
    reset();
}


void MediaFilesPlayer::setTrack(QString filepath)
{
    // Flush old track
    removeTrack();

    // Try to open media file
    try
    {
        // Create new track context
        track = new AudioTrackContext(filepath);

        // Update time slider
        emit signalTime(track->getTime());
        emit signalDuration(track->getDuration());
    }
    catch(const std::exception& e)
    {
        // Error: clear track and notify
        removeTrack();
        emit signalError(e.what());
    }
}


void MediaFilesPlayer::removeTrack()
{
    if (track)
    {
        delete track;
        track = nullptr;
    }
}


void MediaFilesPlayer::setVolume(float volume)
{
    this->volume = volume;
    // Update volume in all opened audio streams
    if (audioVCableSink)
        audioVCableSink->volume(volume);
    if (audioSink)
       audioSink->volume(volume);
}

void MediaFilesPlayer::scheduleState(State state)
{
    if (track)
        scheduledState = state;
}

void MediaFilesPlayer::scheduleTime(double seconds)
{
    if (track)
        scheduledTime = seconds;
}