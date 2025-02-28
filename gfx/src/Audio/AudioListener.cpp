#include "AudioListener.hpp"
#include "Audio.hpp"
#include "../../libs/miniaudioex/include/miniaudioex.h"

namespace GFX
{
    AudioListener::AudioListener() : Component()
    {
        handle = nullptr;
    }

    AudioListener::~AudioListener()
    {
        Audio::Remove(this);
    }

    void AudioListener::OnInitialize()
    {
        handle = ma_ex_audio_listener_init(Audio::GetContext());

        if(handle != nullptr)
        {
            previousPosition = Vector3(0, 0, 0);
            Audio::Add(this);
        }
    }

    void AudioListener::OnDestroy()
    {
        Audio::Remove(this);
    }

    void AudioListener::Destroy()
    {
        if(handle != nullptr)
        {
            ma_ex_audio_listener_uninit(handle);
            handle = nullptr;
        }
    }

    void AudioListener::SetIsEnabled(bool enabled)
    {
        ma_ex_audio_listener_set_spatialization(handle, enabled ? MA_TRUE : MA_FALSE);
    }

    bool AudioListener::GetIsEnabled() const
    {
        return ma_ex_audio_listener_get_spatialization(handle) > 0;
    }

    void AudioListener::SetWorldUp(const Vector3 &worldUp)
    {
        ma_ex_audio_listener_set_world_up(handle, worldUp.x, worldUp.y, worldUp.z);
    }

    Vector3 AudioListener::GetWorldUp() const
    {
        Vector3 worldUp;
        ma_ex_audio_listener_get_world_up(handle, &worldUp.x, &worldUp.y, &worldUp.z);
        return worldUp;
    }

    ma_ex_audio_listener *AudioListener::GetHandle() const
    {
        return handle;
    }
};