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

    // Create audio streams on launch
    mustUpdateDevices = true;

    // Track state update macro
    #define TRACK_STATE_UPDATE(state) \
        track->setState(state); \
        emit signalNewTrackState(state);

    try
    {
        // Track state
        TRACK_STATE_UPDATE(AudioTrackContext::PLAYING)

        // Audio track format
        QAudioFormat format;
        format.setSampleRate(track->getSampleRate());
        format.setChannelCount(track->getChannelCount());
        format.setSampleFormat(QAudioFormat::Float);

        // Player cycle
        while (track->state != AudioTrackContext::STOPPED)
        {
            // Check stream schedule
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
            if (track->state != newTrackState)
            {
                TRACK_STATE_UPDATE(newTrackState)
            }

            if (track->state == AudioTrackContext::PLAYING)
            {
                // Read new frame
                AudioTrackFrame frame = track->readSamples();

                if (frame.size > 0)
                {
                    // Convert frame
                    QByteArray sound = QByteArray((char*)frame.data, frame.size/2);

                    // Wait for availiable space to write data
                    while ((audioVCableSink->bytesFree() < frame.size) || (audioSink->bytesFree() < frame.size)) {}

                    // Write data
                    audioVCableSinkIO->write(sound);
                    audioSinkIO->write(sound);
                }
                else
                {
                    // Wait for the track to finish
                    while ((track->state != AudioTrackContext::STOPPED) &&
                           ((audioVCableSink->bufferSize() != audioVCableSink->bytesFree())
                            || (audioSink->bufferSize() != audioSink->bytesFree()))) {}

                    // Track has ended: update state and notify manager
                    TRACK_STATE_UPDATE(AudioTrackContext::STOPPED)
                }
            }
        }
    }
    catch(const std::exception& e)
    {
        // Error: update track state and notify manager
        TRACK_STATE_UPDATE(AudioTrackContext::STOPPED)
        emit signalError(e.what());
    }

    #undef TRACK_STATE_UPDATE

    // Free streams
    delete audioVCableSink;
    audioVCableSink = nullptr;
    delete audioSink;
    audioSink = nullptr;
}


AudioTrackContext::TrackState MediaFilesPlayer::getTrackState()
{
    return track == nullptr ? AudioTrackContext::STOPPED : track-> state;
}


void MediaFilesPlayer::setNewTrack(QString filepath)
{
    // Manage old track
    delete track;
    // Create new track context
    track = new AudioTrackContext(filepath);
    // Update track state
    emit signalNewTrackState(track->state);
}


void MediaFilesPlayer::setNewTrackState(AudioTrackContext::TrackState state)
{
    newTrackState = state;
}


void MediaFilesPlayer::setNewTrackVolume(float volume)
{
    this->volume = volume;
    // If any audio audio_sink is opened update their volume too
    if (audioVCableSink)
        audioVCableSink->setVolume(volume);
    if (audioSink)
        audioSink->setVolume(volume);
}