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

    #define TRACK_END \
        track->setState(AudioTrackContext::STOPPED);\
        emit signalTrackEnd();

    try
    {
        // Play track
        track->setState(AudioTrackContext::PLAYING);

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

            // Set next track state
            if (track->state != newTrackState)
                track->setState(newTrackState);

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

                    // Stop track
                    track->setState(AudioTrackContext::STOPPED);

                    // Track has ended: update state and notify manager
                    TRACK_END
                }
            }
        }
    }
    catch(const std::exception& e)
    {
        // Update state and notify manager
        TRACK_END
        emit signalError(e.what());
    }

    #undef TRACK_END

    // Free streams
    delete audioVCableSink;
    audioVCableSink = nullptr;
    delete audioSink;
    audioSink = nullptr;
}


void MediaFilesPlayer::setNewTrack(QString filepath)
{
    // Manage old track
    delete track;
    // Create new track context
    track = new AudioTrackContext(filepath);
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