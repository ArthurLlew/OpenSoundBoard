#include "AudioPlayers.hpp"


AudioPlayer::AudioPlayer(QTabWidget const* devices)
{
    this->devices = devices;

    // Do not allow thread pool to destroy us
    setAutoDelete(false);
}


void AudioPlayer::updateAudioStreams()
{
    mustUpdateDevices = true;
}


MicrophonePlayer::MicrophonePlayer(QTabWidget const* devices) : AudioPlayer(devices) {}


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
            // Check stream schedule
            if (mustUpdateDevices)
            {
                delete audioSource;
                audioSource = new AudioSource((DeviceTab*)devices->widget(0));
                delete audioVCableSink;
                audioVCableSink = new AudioSink((DeviceTab*)devices->widget(1), audioSource->format());
                delete audioSink;
                audioSink = new AudioSink((DeviceTab*)devices->widget(2), audioSource->format());
                mustUpdateDevices = false;
            }

            // Check streams
            if (!audioSource->error().isEmpty())
                throw std::runtime_error("Audio input: " + audioSource->error().toStdString());
            if (!audioVCableSink->error().isEmpty())
                throw std::runtime_error("Audio Virtual cable output: " + audioVCableSink->error().toStdString());
            if (!audioSink->error().isEmpty())
                throw std::runtime_error("Audio output: " + audioSink->error().toStdString());


            // Handle input stream (depending on checkbox and stream state)
            if (((DeviceTab*)devices->widget(0))->checkbox->isChecked())
            {
                #define AUDIO_BYTE_SIZE 1024 

                // Read microphone input
                QByteArray audio = audioSource->read(AUDIO_BYTE_SIZE);

                // Wait for availiable space to write data
                while (audioVCableSink->bytesFree(AUDIO_BYTE_SIZE) || audioSink->bytesFree(AUDIO_BYTE_SIZE)) {}

                #undef AUDIO_BYTE_SIZE

                // Write to virtual output stream (depending on checkbox and stream state)
                if (((DeviceTab*)devices->widget(1))->checkbox->isChecked())
                    audioVCableSink->write(audio);

                // Write to output stream (depending on checkbox and stream state)
                if (((DeviceTab*)devices->widget(2))->checkbox->isChecked())
                    audioSink->write(audio);
            }
        }
    }
    catch(const std::exception& e)
    {
        emit signalError(e.what());
    }

    // Free streams
    delete audioSource;
    delete audioVCableSink;
    delete audioSink;
}


void MicrophonePlayer::stop()
{
    isRunning = false;
}


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
                delete audioVCableSink;
                audioVCableSink = new AudioSink((DeviceTab*)devices->widget(1), format);
                audioVCableSink->setVolume(volume);
                delete audioSink;
                audioSink = new AudioSink((DeviceTab*)devices->widget(2), format);
                audioVCableSink->setVolume(volume);
                mustUpdateDevices = false;
            }

            // Check streams
            if (!audioVCableSink->error().isEmpty())
                throw std::runtime_error("Audio Virtual cable output: " + audioVCableSink->error().toStdString());
            if (!audioSink->error().isEmpty())
                throw std::runtime_error("Audio output: " + audioSink->error().toStdString());

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
                    while (audioVCableSink->bytesFree(frame.size) || audioSink->bytesFree(frame.size)) {}

                    // Write to virtual output stream (depending on checkbox)
                    if (((DeviceTab*)devices->widget(1))->checkbox->isChecked())
                        audioVCableSink->write(sound);
                    // Write to output stream
                    audioSink->write(sound);
                }
                else
                {
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
    delete audioSink;
}


void MediaFilesPlayer::setNewTrack(QString filepath)
{
    // Manage old track
    if (track != nullptr)
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