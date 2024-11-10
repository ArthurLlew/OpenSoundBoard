#include "AudioPlayers.hpp"


AudioPlayer::AudioPlayer(QTabWidget const* devices)
{
    this->devices = devices;

    // Do not allow thread pool to destroy us
    setAutoDelete(false);
}


PaStream* AudioPlayer::open_device_stream(DeviceTab const* device_tab, int sample_rate, PaSampleFormat sampleFormat)
{
    // Get currently selected device
    PaDeviceInfo_ext selected_device = device_tab->get_selected_device();
    // Open stream (depends on the device type)
    PaStream *device_stream = NULL;
    PaError res;
    PaStreamParameters stream_prams;
    if (device_tab->device_type == INPUT)
    {
        stream_prams.device = selected_device.index;
        stream_prams.channelCount = selected_device.maxInputChannels;
        stream_prams.sampleFormat = sampleFormat;
        stream_prams.suggestedLatency = selected_device.defaultHighInputLatency;
        stream_prams.hostApiSpecificStreamInfo = NULL;
        res = Pa_OpenStream(&device_stream, &stream_prams, NULL,
                            sample_rate, 1024,
                            paClipOff, NULL, NULL);
    }
    else if (device_tab->device_type == OUTPUT)
    {
        stream_prams.device = selected_device.index;
        stream_prams.channelCount =selected_device.maxOutputChannels;
        stream_prams.sampleFormat = sampleFormat;
        stream_prams.suggestedLatency = selected_device.defaultHighOutputLatency;
        stream_prams.hostApiSpecificStreamInfo = NULL;
        res = Pa_OpenStream(&device_stream, NULL, &stream_prams,
                            sample_rate, 1024,
                            paClipOff, NULL, NULL);
    }

    // Check for arrors
    if (res != paNoError)
    {
        throw runtime_error("Unable to open stream");
    }

    // Start stream
    Pa_StartStream(device_stream);

    return device_stream;
}


void AudioPlayer::kill()
{
    is_alive = false;
}


MicrophonePlayer::MicrophonePlayer(QTabWidget const* devices) : AudioPlayer(devices){}


void MicrophonePlayer::run()
{
    is_alive = true;

    // Audio buffer
    unsigned long size = 1024;
    void *buff = NULL;
    // Audio streams
    PaStream *in_stream = NULL, *virtual_out_stream = NULL, *out_stream = NULL;
    
    try
    {
        // Allocate audio buffer
        buff = malloc(sizeof(paInt16)*size);

        // Open relevant streams
        in_stream = open_device_stream((DeviceTab*)devices->widget(0), 48000, paInt16);
        virtual_out_stream = open_device_stream((DeviceTab*)devices->widget(1), 48000, paInt16);
        out_stream = open_device_stream((DeviceTab*)devices->widget(2), 48000, paInt16);

        // Player cycle
        while (is_alive && Pa_IsStreamActive(in_stream))
        {
            // Handle input stream (depending on checkbox and stream state)
            if (((DeviceTab*)devices->widget(0))->checkbox->isChecked() && Pa_IsStreamActive(in_stream))
            {
                // Read microphone input
                Pa_ReadStream(in_stream, buff, size);

                // Write to virtual output stream (depending on checkbox and stream state)
                if (((DeviceTab*)devices->widget(1))->checkbox->isChecked() && Pa_IsStreamActive(virtual_out_stream))
                    Pa_WriteStream(virtual_out_stream, buff, size);

                // Write to output stream (depending on checkbox and stream state)
                if (((DeviceTab*)devices->widget(2))->checkbox->isChecked() && Pa_IsStreamActive(out_stream))
                    Pa_WriteStream(out_stream, buff, size);
            }
        }
    }
    catch(const std::exception& e)
    {
        is_alive = false;
        emit player_error(e.what());
    }

    // Free buffer if allocated
    if(buff)
        free(buff);
    // Free streams if allocated
    if (in_stream)
    {
        Pa_AbortStream(in_stream);
        Pa_CloseStream(in_stream);
    }
    if (virtual_out_stream)
    {
        Pa_AbortStream(virtual_out_stream);
        Pa_CloseStream(virtual_out_stream);
    }
    if (out_stream)
    {
        Pa_AbortStream(out_stream);
        Pa_CloseStream(out_stream);
    }
}


MediaFilesPlayer::MediaFilesPlayer(QTabWidget const *devices, float const *volume_ptr) : AudioPlayer(devices)
{
    this->volume_ptr = volume_ptr;
}


MediaFilesPlayer::~MediaFilesPlayer()
{
    delete track;
}


void MediaFilesPlayer::run()
{
    is_alive = true;

    // Audio streams
    PaStream *virtual_out_stream_44100 = NULL, *out_stream_44100 = NULL,
             *virtual_out_stream_48000 = NULL, *out_stream_48000 = NULL;

    try
    {
        // Open relevant streams
        virtual_out_stream_44100 = open_device_stream((DeviceTab*)devices->widget(1), 44100, paFloat32);
        out_stream_44100 = open_device_stream((DeviceTab*)devices->widget(2), 44100, paFloat32);
        virtual_out_stream_48000 = open_device_stream((DeviceTab*)devices->widget(1), 48000, paFloat32);
        out_stream_48000 = open_device_stream((DeviceTab*)devices->widget(2), 48000, paFloat32);

        // Player cycle
        while (is_alive)
        {
            // Check for the first playing file
            if (track != nullptr)
            {
                // Set next track state
                if (track->state != next_track_state)
                    track->set_state(next_track_state);

                if (track->state == PLAYING)
                {
                    AudioTrackFrame frame = track->read_samples(*volume_ptr);

                    if (frame.nb_samples > 0)
                    {
                        switch (frame.sample_rate)
                        {
                            case 44100:
                                // Write to virtual output stream (depending on checkbox)
                                if (((DeviceTab*)devices->widget(1))->checkbox->isChecked())
                                    Pa_WriteStream(virtual_out_stream_44100, frame.data, frame.nb_samples);
                                // Write to output stream
                                Pa_WriteStream(out_stream_44100, frame.data, frame.nb_samples);
                                break;
                            case 48000:
                                // Write to virtual output stream (depending on checkbox)
                                if (((DeviceTab*)devices->widget(1))->checkbox->isChecked())
                                    Pa_WriteStream(virtual_out_stream_48000, frame.data, frame.nb_samples);
                                // Write to output stream
                                Pa_WriteStream(out_stream_48000, frame.data, frame.nb_samples);
                                break;
                            default:
                                throw runtime_error("Unsupported samples rate");
                        }
                    }
                    else
                    {
                        // Track has ended: update state and notify manager
                        next_track_state = STOPPED;
                        emit track_endeded();
                    }
                }
            }
        }
    }
    catch(const std::exception& e)
    {
        is_alive = false;
        emit player_error(e.what());
    }

    // Free streams if allocated
    if (virtual_out_stream_44100)
    {
        Pa_AbortStream(virtual_out_stream_44100);
        Pa_CloseStream(virtual_out_stream_44100);
    }
    if (out_stream_44100)
    {
        Pa_AbortStream(out_stream_44100);
        Pa_CloseStream(out_stream_44100);
    }if (virtual_out_stream_48000)
    {
        Pa_AbortStream(virtual_out_stream_48000);
        Pa_CloseStream(virtual_out_stream_48000);
    }
    if (out_stream_48000)
    {
        Pa_AbortStream(out_stream_48000);
        Pa_CloseStream(out_stream_48000);
    }
}


void MediaFilesPlayer::new_track(QString filepath)
{
    // Manage old track
    if (track != nullptr)
        delete track;
    // Create new track context
    track = new AudioTrackContext(filepath);
}


void MediaFilesPlayer::new_track_state(TrackState track_state)
{
    next_track_state = track_state;
}