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
        setTrackState(PLAYING);

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
            if (state != newTrackState)
            {
                setTrackState(newTrackState);
            }

            if (state == PLAYING)
            {
                // Read new frame
                AudioTrackFrame frame = track->readSample();

                if (frame.size > 0)
                {
                    // Convert frame
                    QByteArray sound = QByteArray((char*)frame.data, frame.size/track->getChannelCount());

                    // Wait for availiable space and write data
                    while ((audioVCableSink->bytesFree() < frame.size) || (audioSink->bytesFree() < frame.size)) {}
                    audioVCableSinkIO->write(sound);
                    audioSinkIO->write(sound);
                }
                else
                {
                    // Wait for the track to finish
                    while (audioVCableSink->bufferSize() != audioVCableSink->bytesFree()
                           || audioSink->bufferSize() != audioSink->bytesFree()) {}

                    // Track has ended: update state
                    setTrackState(STOPPED);
                }
            }
        }
    }
    catch(const std::exception& e)
    {
        // Error: update track state
        setTrackState(STOPPED);
        emit signalError(e.what());
    }

    // Stop streams
    stopAudioStream(&audioVCableSink);
    stopAudioStream(&audioSink);
    // Raise update flag
    mustUpdateDevices = true;
}


MediaFilesPlayer::TrackState MediaFilesPlayer::getTrackState()
{
    return track == nullptr ? STOPPED : state;
}


void MediaFilesPlayer::setTrackState(TrackState state)
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
        newTrackState = state;

        emit updateTrackState(this->state);
    }
}

void MediaFilesPlayer::nextTrackState(TrackState state)
{
    if (track != nullptr)
        newTrackState = state;
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


void MediaFilesPlayer::setTrackVolume(qreal volume)
{
    this->volume = volume;
    // Update volume in any opened audio sink
    if (audioVCableSink)
        audioVCableSink->setVolume(volume);
    if (audioSink)
        audioSink->setVolume(volume);
}