#ifndef DEVICE_STREAM
#define DEVICE_STREAM


// Exceptions
#include <stdexcept>
// SDL3
#include <SDL3/SDL.h>


/**
 * Stores SDL device stream.
 */
class DeviceStream
{
    // Device stream.
    SDL_AudioStream *audio_stream = nullptr;
    // Device audio format
    SDL_AudioSpec audio_format;

public:
    /**
     * Constructor. Inits and starts audio stream with native format.
     */
    DeviceStream(SDL_AudioDeviceID device_id)
    {
        // Init audio stream with native (for selected device) format
        audio_stream = SDL_OpenAudioDeviceStream(device_id, NULL, NULL, NULL);
        // Proceed or throw
        if (audio_stream)
        {
            // Get audio format
            SDL_GetAudioStreamFormat(audio_stream, NULL, &audio_format);
            // Start audio stream
            SDL_ResumeAudioStreamDevice(audio_stream);
        }
        else
        {
            throw std::runtime_error("Audio device: unable to create stream");
        }
    }

    /**
     * Constructor. Inits and starts audio stream with provided format.
     */
    DeviceStream(SDL_AudioDeviceID device_id, SDL_AudioSpec audio_format)
    {
        // Save audio format
        this->audio_format = audio_format;
        // Init audio stream with provided format
        audio_stream = SDL_OpenAudioDeviceStream(device_id, &this->audio_format, NULL, NULL);
        // Proceed or throw
        if (audio_stream)
        {
            // Start audio stream
            SDL_ResumeAudioStreamDevice(audio_stream);
        }
        else
        {
            throw std::runtime_error("Audio device: unable to create stream");
        }
    }

    /**
     * Destructor. Closes and frees audio stream.
     */
    ~DeviceStream()
    {
        // Destroy audio stream
        SDL_DestroyAudioStream(audio_stream);
    }

    /**
     * @return audio stream
     */
    SDL_AudioStream* stream()
    {
        return audio_stream;
    }

    /**
     * @return audio format
     */
    SDL_AudioSpec format() const
    {
        return audio_format;
    }

    /**
     * @return whether stream has no audio data in queue
     */
    bool isEmpty()
    {
        return SDL_GetAudioStreamQueued(audio_stream) == 0;
    }

    /**
     * @return whether stream is ready to recieve audio
     */
    bool isReadyForWrite(int size)
    {
        // Limit size by factor 3 from standard buffer size of 1024
        # define MAX_AUDIO_BUFFER_SIZE 8*1024
        return (size * SDL_AUDIO_FRAMESIZE(audio_format)) < (MAX_AUDIO_BUFFER_SIZE * SDL_AUDIO_FRAMESIZE(audio_format) - SDL_GetAudioStreamQueued(audio_stream));
    }

    /**
     * Sets current volume of stream
     */
    void volume(float value)
    {
        SDL_SetAudioStreamGain(audio_stream, value);
    }

    /**
     * Reads audio data from stream.
     * 
     * @param buffer audio data buffer
     * @param size size of data in samples
     */
    void read(void *buffer, int size)
    {
        // If have enough data
        if (SDL_GetAudioStreamQueued(audio_stream) >= size)
        {
            // Read
            SDL_GetAudioStreamData(audio_stream, buffer, size * SDL_AUDIO_FRAMESIZE(audio_format));
        }
    }

    /**
     * Writes audio data to stream.
     * 
     * @param buffer audio data buffer
     * @param size size of data in samples
     */
    void write(const void *buffer, int size)
    {
        // Send data
        SDL_PutAudioStreamData(audio_stream, buffer, size * SDL_AUDIO_FRAMESIZE(audio_format));
    }

    /**
     * Flushes stream indicating end of data
     */
    void flush()
    {
        SDL_FlushAudioStream(audio_stream);
    }
};


#endif // DEVICE_STREAM