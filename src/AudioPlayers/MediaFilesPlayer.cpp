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

        // Audio track format
        QAudioFormat format;
        format.setSampleRate(track->getSampleRate());
        format.setChannelCount(track->getChannelCount());
        format.setSampleFormat(QAudioFormat::Float);

        // Player cycle
        while (state != STOPPED)
        {
            // Update streams if needed
            if (mustUpdateDevices)
            {
                audioVCableSinkIO = restartAudioSink(&audioVCableSink, (DeviceTab*)devices->widget(1), format);
                audioVCableSink->setVolume(volume);
                audioSinkIO = restartAudioSink(&audioSink, (DeviceTab*)devices->widget(2), format);
                audioSink->setVolume(volume);
                mustUpdateDevices = false;
            }

            // Check streams
            if (audioVCableSink->state() == QtAudio::StoppedState)
                throw std::runtime_error("Audio Virtual cable output suddenly stopped");
            if (audioSink->state() == QtAudio::StoppedState)
                throw std::runtime_error("Audio output suddenly stopped");

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
                    // Calculate size in bytes
                    int sizeInBytes = track->getAudioDataSamplesCount() * track->getChannelCount() * sizeof(float);

                    // Wait for availiable space and write data
                    if ((audioVCableSink->bytesFree() >= sizeInBytes) && (audioSink->bytesFree() >= sizeInBytes))
                    {
                        audioVCableSinkIO->write((const char*)track->getAudioData()[0], sizeInBytes);
                        audioSinkIO->write((const char*)track->getAudioData()[0], sizeInBytes);
                        shouldReadSamples = true;
                    }
                }
                else
                {
                    // If all previous data was consumed by IOs
                    if ((audioVCableSink->bufferSize() == audioVCableSink->bytesFree()) && (audioSink->bufferSize() == audioSink->bytesFree()))
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
    // Raise update flags
    mustUpdateDevices = true;
    shouldReadSamples = true;
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


void MediaFilesPlayer::setVolume(qreal volume)
{
    this->volume = volume;
    // Update volume in any opened audio sink
    if (audioVCableSink)
        audioVCableSink->setVolume(volume);
    if (audioSink)
        audioSink->setVolume(volume);
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