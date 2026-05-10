#include <AudioPlayers/MediaFilesPlayer.hpp>

MediaFilesPlayer::MediaFilesPlayer(QTabWidget const *devices) : AudioPlayer(devices) {}


MediaFilesPlayer::~MediaFilesPlayer()
{
    delete track;
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

            // Set scheduled track state
            if (state != nextTrackState)
            {
                setState(nextTrackState);
            }

            if (state == PLAYING)
            {
                // Read samples
                int samplesCount = track->read();

                // If sample count is positive
                if (samplesCount > 0)
                {
                    // Calculate size in bytes
                    int sizeInBytes = samplesCount * track->getChannelCount() * sizeof(float);

                    // Wait for availiable space and write data
                    while (audioVCableSink->bytesFree() < sizeInBytes || audioSink->bytesFree() < sizeInBytes) {}
                    audioVCableSinkIO->write((const char*)track->getAudioData()[0], sizeInBytes);
                    audioSinkIO->write((const char*)track->getAudioData()[0], sizeInBytes);
                }
                else
                {
                    // Wait for the track to finish
                    while (audioVCableSink->bufferSize() != audioVCableSink->bytesFree()
                           || audioSink->bufferSize() != audioSink->bytesFree()) {}

                    // Track has ended: update state
                    setState(STOPPED);
                }
            }
        }
    }
    catch(const std::exception& e)
    {
        // Error: update track state
        setState(STOPPED);
        emit signalError(e.what());
    }

    // Stop streams
    stopAudioStream(&audioVCableSink);
    stopAudioStream(&audioSink);
    // Raise update flag
    mustUpdateDevices = true;
}


void MediaFilesPlayer::setTrack(QString filepath)
{
    // Manage old track
    if (track != nullptr)
    {
        delete track;
        track = nullptr;
    }

    // Create new track context
    track = new AudioTrackContext(filepath);
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


void MediaFilesPlayer::setState(State state)
{
    if (track != nullptr)
    {
        // Close track if set to stop
        if (this->state == PLAYING || this->state == PAUSED && state == STOPPED)
        {
            track->close();
        }
        // Start track if was stopped
        if (this->state == STOPPED && state == PLAYING || state == PAUSED)
        {
            track->open();
        }

        // Update state
        this->state = state;
        nextTrackState = state;

        emit signalState(this->state);
    }
}

void MediaFilesPlayer::setPlannedState(State state)
{
    if (track != nullptr)
        nextTrackState = state;
}