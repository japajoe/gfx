#include "Audio.hpp"
#include "../../libs/miniaudioex/include/miniaudioex.h"
#include "../Core/Transform.hpp"

namespace GFX
{
    ma_ex_context *Audio::context = nullptr;
    std::vector<AudioSource*> Audio::sources;
    std::vector<AudioListener*> Audio::listeners;
    int32_t Audio::sampleRate = 44100;
    int32_t Audio::channels = 2;

    void Audio::Initialize(uint32_t sampleRate, uint32_t channels)
    {
        Audio::sampleRate = sampleRate;
        Audio::channels = channels;

        ma_ex_context_config config = ma_ex_context_config_init(sampleRate, channels, 2048, NULL);
        context = ma_ex_context_init(&config);
    }

    void Audio::Deinitialize()
    {
        if(context != nullptr)
        {
            for(size_t i = 0; i < sources.size(); i++)
                sources[i]->Destroy();

            sources.clear();

            for(size_t i = 0; i < listeners.size(); i++)
                listeners[i]->Destroy();

            listeners.clear();

            ma_ex_context_uninit(context);
            context = nullptr;
        }
    }

    void Audio::NewFrame()
    {
        if(context == nullptr)
            return;

        for(size_t i = 0; i < listeners.size(); i++)
        {
            auto handle = listeners[i]->GetHandle();

            auto position = listeners[i]->GetTransform()->GetPosition();
            auto direction = listeners[i]->GetTransform()->GetForward();
            auto velocity = listeners[i]->GetTransform()->GetVelocity();

            ma_ex_audio_listener_set_position(handle, position.x, position.y, position.z);
            ma_ex_audio_listener_set_direction(handle, direction.x, direction.y, direction.z);
            ma_ex_audio_listener_set_velocity(handle, velocity.x, velocity.y, velocity.z);
        }

        for(size_t i = 0; i < sources.size(); i++)
        {
            sources[i]->Update();

            if(!sources[i]->GetSpatial())
                continue;
            
            auto handle = sources[i]->GetHandle();
            auto position = sources[i]->GetTransform()->GetPosition();
            auto direction = sources[i]->GetTransform()->GetForward();
            auto velocity = sources[i]->GetTransform()->GetVelocity();

            ma_ex_audio_source_set_position(handle, position.x, position.y, position.z);
            ma_ex_audio_source_set_direction(handle, direction.x, direction.y, direction.z);
            ma_ex_audio_source_set_velocity(handle, velocity.x, velocity.y, velocity.z);
        }
    }

    ma_ex_context *Audio::GetContext()
    {
        return context;
    }

    void Audio::Add(AudioSource *source)
    {
        for(size_t i = 0; i < sources.size(); i++)
        {
            if(sources[i] == source)
                return;
        }

        sources.push_back(source);
    }

    void Audio::Add(AudioListener *listener)
    {
        for(size_t i = 0; i < listeners.size(); i++)
        {
            if(listeners[i] == listener)
                return;
        }

        listeners.push_back(listener);
    }

    void Audio::Remove(AudioSource *source)
    {
        bool found = false;
        size_t index = 0;

        for(size_t i = 0; i < sources.size(); i++)
        {
            if(sources[i] == source)
            {
                index = i;
                found = true;
                break;
            }
        }

        if(found)
        {
            sources[index]->Destroy();
            sources.erase(sources.begin() + index);
        }
    }

    void Audio::Remove(AudioListener *listener)
    {
        bool found = false;
        size_t index = 0;

        for(size_t i = 0; i < listeners.size(); i++)
        {
            if(listeners[i] == listener)
            {
                index = i;
                found = true;
                break;
            }
        }

        if(found)
        {
            listeners[index]->Destroy();
            listeners.erase(listeners.begin() + index);
        }
    }

    int32_t Audio::GetSampleRate()
    {
        return sampleRate;
    }

    int32_t Audio::GetChannels()
    {
        return channels;
    }

    void Audio::SetMasterVolume(float volume)
    {
        if(context == nullptr)
            return;
        ma_ex_context_set_master_volume(context, volume);
    }

    float Audio::GetMasterVolume()
    {
        if(context == nullptr)
            return 0.0f;
        return ma_ex_context_get_master_volume(context);
    }
};