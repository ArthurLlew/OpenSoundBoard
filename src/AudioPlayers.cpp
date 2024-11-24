#include "AudioPlayers.hpp"


AudioPlayer::AudioPlayer(QTabWidget const* devices)
{
    this->devices = devices;

    // Do not allow thread pool to destroy us
    setAutoDelete(false);
}


void AudioPlayer::stop()
{
    is_alive = false;
}


MicrophonePlayer::MicrophonePlayer(QTabWidget const* devices) : AudioPlayer(devices) {}


void MicrophonePlayer::run()
{
    is_alive = true;

    // Audio streams
    QAudioSource *audio_source = nullptr;
    QAudioSink *audio_sink_cable = nullptr, *audio_sink = nullptr;
    
    try
    {
        // Open relevant streams (output streams inherit input stream format, so check if it is supported)
        QAudioDevice audio_source_device = ((DeviceTab*)devices->widget(0))->getDevice();
        QAudioFormat audio_source_format = audio_source_device.preferredFormat();
        audio_source = new QAudioSource(audio_source_device, audio_source_format);
        QAudioDevice audio_sink_cable_device = ((DeviceTab*)devices->widget(1))->getDevice();
        if (audio_sink_cable_device.isFormatSupported(audio_source_format))
        {
            audio_sink_cable = new QAudioSink(audio_sink_cable_device, audio_source_format);
        }
        else
        {
            throw std::runtime_error("Virtual output device does not support input device audio format");
        }
        QAudioDevice audio_sink_device = ((DeviceTab*)devices->widget(2))->getDevice();
        if (audio_sink_device.isFormatSupported(audio_source_format))
        {
            audio_sink = new QAudioSink(audio_sink_device, audio_source_format);
        }
        else
        {
            throw std::runtime_error("Output device does not support input device audio format");
        }
        // Start streams and get their IODevices
        QIODevice *audio_source_io = audio_source->start();
        QIODevice *audio_sink_cable_io = audio_sink_cable->start();
        QIODevice *sink_io = audio_sink->start();

        // Player cycle
        while (is_alive && (audio_source->state() != QtAudio::StoppedState))
        {
            // Handle input stream (depending on checkbox and stream state)
            if (((DeviceTab*)devices->widget(0))->checkbox->isChecked() && (audio_source->state() != QtAudio::StoppedState))
            {
                #define SOUND_BYTE_SIZE 1024 

                // Read microphone input
                QByteArray sound = audio_source_io->read(SOUND_BYTE_SIZE);

                // Wait for availiable space to write data
                while ((audio_sink_cable->bytesFree() < SOUND_BYTE_SIZE) || (audio_sink->bytesFree() < SOUND_BYTE_SIZE)) {}

                #undef SOUND_BYTE_SIZE

                // Write to virtual output stream (depending on checkbox and stream state)
                if (((DeviceTab*)devices->widget(1))->checkbox->isChecked() && (audio_sink_cable->state() != QtAudio::StoppedState))
                    audio_sink_cable_io->write(sound);

                // Write to output stream (depending on checkbox and stream state)
                if (((DeviceTab*)devices->widget(2))->checkbox->isChecked() && (audio_sink->state() != QtAudio::StoppedState))
                    sink_io->write(sound);
            }
        }
    }
    catch(const std::exception& e)
    {
        emit signalError(e.what());
    }

    // Free streams if allocated
    if (audio_source)
    {
        audio_source->stop();
        delete audio_source;
    }
    if (audio_sink_cable)
    {
        audio_sink_cable->stop();
        delete audio_sink_cable;
    }
    if (audio_sink)
    {
        audio_sink->stop();
        delete audio_sink;
    }

    is_alive = false;
}


MediaFilesPlayer::MediaFilesPlayer(QTabWidget const *devices) : AudioPlayer(devices) {}


MediaFilesPlayer::~MediaFilesPlayer()
{
    delete track;
}


void MediaFilesPlayer::run()
{
    if (track == nullptr)
        return;

    is_alive = true;

    track->setState(AudioTrackContext::PLAYING);

    // Audio format
    QAudioFormat format;
    format.setSampleRate(track->getSampleRate());
    format.setChannelCount(track->getChannelCount());
    format.setSampleFormat(QAudioFormat::Float);

    try
    {
        // Open relevant streams
        audio_sink_cable = new QAudioSink(((DeviceTab*)devices->widget(1))->getDevice(), format);
        audio_sink = new QAudioSink(((DeviceTab*)devices->widget(2))->getDevice(), format);
        // Start streams and get their IODevices
        QIODevice *audio_sink_cable_io = audio_sink_cable->start();
        audio_sink_cable->setVolume(volume);
        QIODevice *sink_io = audio_sink->start();
        audio_sink->setVolume(volume);

        // Player cycle
        while (is_alive)
        {
            // Set next track state
            if (track->state != nextTrackState)
                track->setState(nextTrackState);

            if (track->state == AudioTrackContext::PLAYING)
            {
                // Read new frame
                AudioTrackFrame frame = track->readSamples();

                if (frame.size > 0)
                {
                    // Convert frame
                    QByteArray sound = QByteArray((char*)frame.data, frame.size/2);

                    // Wait for availiable space to write data
                    while ((audio_sink_cable->bytesFree() < frame.size) || (audio_sink->bytesFree() < frame.size)) {}

                    // Write to virtual output stream (depending on checkbox)
                    if (((DeviceTab*)devices->widget(1))->checkbox->isChecked())
                        audio_sink_cable_io->write(sound);
                    // Write to output stream
                    sink_io->write(sound);
                }
                else
                {
                    // Track has ended: update state and notify manager
                    nextTrackState = AudioTrackContext::STOPPED;
                    emit signalTrackEnd();
                }
            }
        }
    }
    catch(const std::exception& e)
    {
        nextTrackState = AudioTrackContext::STOPPED;
        emit signalTrackEnd();
        emit signalError(e.what());
    }

    // Free streams if allocated
    if (audio_sink_cable)
    {
        audio_sink_cable->stop();
        delete audio_sink_cable;
    }
    if (audio_sink)
    {
        audio_sink->stop();
        delete audio_sink;
    }

    is_alive = false;
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
    nextTrackState = state;
}


void MediaFilesPlayer::setNewTrackVolume(float volume)
{
    this->volume = volume;
    // If any audio audio_sink is opened update their volume too
    if (audio_sink_cable)
        audio_sink_cable->setVolume(volume);
    if (audio_sink)
        audio_sink->setVolume(volume);
}