#ifndef DEVICES_LIST
#define DEVICES_LIST


// SDL3
#include <SDL3/SDL.h>


/**
 * Stores SDL devices list.
 */
class DevicesList
{
public:
    /**
     * Describes audio device type.
     */ 
    enum DeviceType
    {
        INPUT,
        OUTPUT
    };

private:
    // Device type.
    DeviceType device_type;

    // Audio devices list
    SDL_AudioDeviceID* devices = nullptr;
    // Audio devices count
    int devices_count = 0;

public:
    /**
     * Constructor. Inits SDL devices list.
     */
    DevicesList(DeviceType device_type)
    {
        // Save device type
        this->device_type = device_type;
    }

    /**
     * Destructor. Clears SDL devices list.
     */
    ~DevicesList()
    {
        if (devices)
            SDL_free(devices);
    }

    /**
     * @return audio devices type
     */
    DeviceType type()
    {
        return device_type;
    }

    void refresh()
    {
        // Free old list
        if (devices)
        {
            devices_count = 0;
            SDL_free(devices);
        }

        // Create new list depending on type
        switch (device_type)
        {
            case INPUT:
            {
                devices = SDL_GetAudioRecordingDevices(&devices_count);
                break;
            }
            case OUTPUT:
            {
                devices = SDL_GetAudioPlaybackDevices(&devices_count);
                break;
            }
        }
    }

    /**
     * @return audio devices count
     */
    int count() const
    {
        return devices_count;
    }

    /**
     * @return audio device ID
     */
    SDL_AudioDeviceID get(int index)
    {
        return devices[index];
    }
};


#endif // DEVICES_LIST