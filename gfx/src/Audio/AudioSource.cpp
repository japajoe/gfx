#include "AudioSource.hpp"
#include "Audio.hpp"
#include "../../libs/miniaudioex/include/miniaudioex.h"
#include <iostream>

namespace GFX
{
    AudioSource::AudioSource() : Component()
    {
        handle = nullptr;
    }

    AudioSource::~AudioSource()
    {
        Audio::Remove(this);
    }

    void AudioSource::OnInitialize()
    {
        handle = ma_ex_audio_source_init(Audio::GetContext());

        if(handle != nullptr)
        {
            hasEnded.store(false);

            previousPosition = Vector3(0, 0, 0);
            
            ma_ex_audio_source_callbacks callbacks;
            callbacks.pUserData = this;
            callbacks.endCallback = OnAudioEnded;
            callbacks.loadCallback = OnAudioLoaded;
            callbacks.processCallback = OnAudioProcess;
            callbacks.waveformCallback = OnAudioRead;

            ma_ex_audio_source_set_callbacks(handle, callbacks);

            Audio::Add(this);
        }
    }

    void AudioSource::OnDestroy()
    {
        Audio::Remove(this);
    }

    void AudioSource::Update()
    {
        if(hasEnded == true)
        {
            hasEnded.store(false);
            end(this);
        }
    }

    void AudioSource::Stop()
    {
        ma_ex_audio_source_stop(handle);
    }

    void AudioSource::Play(const AudioClip *clip)
    {
        if(clip != nullptr)
        {
            ma_result result = MA_ERROR;

            if(clip->GetHandle() != nullptr)
                result = ma_ex_audio_source_play_from_memory(handle, clip->GetHandle(), clip->GetDataSize());
            else
                result = ma_ex_audio_source_play_from_file(handle, clip->GetFilePath().c_str(), clip->GetStreamFromDisk() ? MA_TRUE : MA_FALSE);

            if(result != MA_SUCCESS)
                printf("Failed to play audio: %d\n", result);
        }
    }

    void AudioSource::Play()
    {
        ma_ex_audio_source_play(handle);
    }

    void AudioSource::Destroy()
    {
        if(handle != nullptr)
        {
            ma_ex_audio_source_stop(handle);
            ma_ex_audio_source_uninit(handle);
            handle = nullptr;

            for(size_t i = 0; i < effects.GetCount(); i++)
                effects[i]->OnDestroy();

            for(int i = 0; i < generators.GetCount(); i++)
                generators[i]->OnDestroy();
        }
    }

    bool AudioSource::IsPlaying() const
    {
        return ma_ex_audio_source_get_is_playing(handle) > 0;
    }

    void AudioSource::SetIsLooping(bool loop)
    {
        ma_ex_audio_source_set_loop(handle, loop ? MA_TRUE : MA_FALSE);
    }

    bool AudioSource::GetIsLooping() const
    {
        return ma_ex_audio_source_get_loop(handle) > 0;
    }

    void AudioSource::SetVolume(float volume)
    {
        return ma_ex_audio_source_set_volume(handle, volume);
    }

    float AudioSource::GetVolume() const
    {
        return ma_ex_audio_source_get_volume(handle);
    }

    void AudioSource::SetPitch(float pitch)
    {
        return ma_ex_audio_source_set_pitch(handle, pitch);
    }

    float AudioSource::GetPitch() const
    {
        return ma_ex_audio_source_get_pitch(handle);
    }

    void AudioSource::SetCursor(UInt64 value)
    {
        ma_ex_audio_source_set_pcm_position(handle, value);
    }

    UInt64 AudioSource::GetCursor() const
    {
        return ma_ex_audio_source_get_pcm_position(handle);
    }

    UInt64 AudioSource::GetClipLength() const
    {
        return ma_ex_audio_source_get_pcm_length(handle);
    }

    void AudioSource::SetSpatial(bool spatial)
    {
        ma_ex_audio_source_set_spatialization(handle, spatial ? MA_TRUE : MA_FALSE);
    }

    bool AudioSource::GetSpatial() const
    {
        return ma_ex_audio_source_get_spatialization(handle) > 0;
    }

    void AudioSource::SetDopplerFactor(float factor)
    {
        ma_ex_audio_source_set_doppler_factor(handle, factor);
    }

    float AudioSource::GetDopplerFactor() const
    {
        return ma_ex_audio_source_get_doppler_factor(handle);
    }

    void AudioSource::SetAttenuationModel(AttenuationModel model)
    {
        ma_ex_audio_source_set_attenuation_model(handle, static_cast<ma_attenuation_model>(model));
    }

    AttenuationModel AudioSource::GetAttenuationModel() const
    {
        auto model = ma_ex_audio_source_get_attenuation_model(handle);
        return static_cast<AttenuationModel>(model);
    }

    void AudioSource::SetMinDistance(float distance)
    {
        ma_ex_audio_source_set_min_distance(handle, distance);
    }

    float AudioSource::GetMinDistance() const
    {
        return ma_ex_audio_source_get_min_distance(handle);
    }

    void AudioSource::SetMaxDistance(float distance)
    {
        ma_ex_audio_source_set_max_distance(handle, distance);
    }

    float AudioSource::GetMaxDistance() const
    {
        return ma_ex_audio_source_get_max_distance(handle);
    }

    ma_ex_audio_source *AudioSource::GetHandle() const
    {
        return handle;
    }

    void AudioSource::OnAudioEnded(void *pUserData, ma_sound *pSound)
    {
        AudioSource *pSource = reinterpret_cast<AudioSource*>(pUserData);
        if(pSource)
            pSource->hasEnded.store(true);
    }

    void AudioSource::OnAudioLoaded(void *pUserData, ma_sound *pSound)
    {
        AudioSource *pSource = reinterpret_cast<AudioSource*>(pUserData);
        if(pSource)
            pSource->load(pSource);
    }

    void AudioSource::OnAudioProcess(void* pUserData, ma_sound* pSound, float* pFramesOut, UInt64 frameCount, UInt32 channels)
    {
        AudioSource *pSource = reinterpret_cast<AudioSource*>(pUserData);
        if(pSource)
        {
            float *pData = reinterpret_cast<float*>(pFramesOut);

            AudioBuffer<float> buffer(pData, frameCount * channels);

            for(size_t i = 0; i < pSource->effects.GetCount(); i++)
            {
                pSource->effects[i]->OnProcess(buffer, frameCount, channels);
            }

            pSource->process(pSource, buffer, frameCount, channels);
        }
    }

    void AudioSource::OnAudioRead(void *pUserData, void* pFramesOut, UInt64 frameCount, UInt32 channels)
    {
        AudioSource *pSource = reinterpret_cast<AudioSource*>(pUserData);
        if(pSource)
        {
            float *pData = reinterpret_cast<float*>(pFramesOut);

            AudioBuffer<float> buffer(pData, frameCount * channels);

            for(size_t i = 0; i < pSource->generators.GetCount(); i++)
            {
                pSource->generators[i]->OnGenerate(buffer, frameCount, channels);
            }

            pSource->read(pSource, buffer, frameCount, channels);
        }
    }
};