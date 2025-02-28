#ifndef GFX_AUDIOSOURCE_HPP
#define GFX_AUDIOSOURCE_HPP

#include "AudioClip.hpp"
#include "AudioBuffer.hpp"
#include "DSP/AudioEffect.hpp"
#include "DSP/AudioGenerator.hpp"
#include "../Core/Component.hpp"
#include "../System/Collections/ConcurrentList.hpp"
#include "../System/EventHandler.hpp"
#include "../System/Numerics/Vector3.hpp"
#include <memory>
#include <type_traits> // For std::enable_if, std::is_base_of
#include <atomic>
#include <cstdint>
#include <vector>

struct ma_ex_audio_source;
struct ma_sound;
typedef unsigned long long UInt64;
typedef unsigned int UInt32;

namespace GFX
{
    class AudioSource;

    using AudioEndedCallback = std::function<void(AudioSource*)>;
    using AudioLoadedCallback = std::function<void(AudioSource*)>;
    using AudioProcessCallback = std::function<void(AudioSource*,AudioBuffer<float>, UInt64, UInt32)>;
    using AudioReadCallback = std::function<void(AudioSource*,AudioBuffer<float>, UInt64, UInt32)>;

    enum class AttenuationModel
    {
        None,
        Inverse,
        Linear,
        Exponential
    };

    class AudioSource : public Component
    {
    friend class Audio;
    public:
        EventHandler<AudioEndedCallback> end;
        EventHandler<AudioLoadedCallback> load;
        EventHandler<AudioProcessCallback> process;
        EventHandler<AudioReadCallback> read;
        AudioSource();
        ~AudioSource();
        void Update();
        void Stop();
        void Play();
        void Play(const AudioClip *clip);            
        bool IsPlaying() const;
        void SetIsLooping(bool loop);
        bool GetIsLooping() const;
        void SetVolume(float volume);
        float GetVolume() const;
        void SetPitch(float pitch);
        float GetPitch() const;
        void SetCursor(UInt64 value);
        UInt64 GetCursor() const;
        UInt64 GetClipLength() const;
        void SetSpatial(bool spatial);
        bool GetSpatial() const;
        void SetDopplerFactor(float factor);
        float GetDopplerFactor() const;
        void SetAttenuationModel(AttenuationModel model);
        AttenuationModel GetAttenuationModel() const;
        void SetMinDistance(float distance);
        float GetMinDistance() const;
        void SetMaxDistance(float distance);
        float GetMaxDistance() const;
        ma_ex_audio_source *GetHandle() const;

        template<typename T, typename... Param>
        T* AddEffect(Param&&... param) 
        {
            static_assert(std::is_base_of<AudioEffect, T>::value, "AddEffect parameter must derive from AudioEffect");

            std::unique_ptr<AudioEffect> ptr = std::make_unique<T>(std::forward<Param>(param)...);
            if (!ptr)
                return nullptr;

            effects.Add(std::move(ptr));
            size_t last = effects.GetCount() - 1;
            AudioEffect* effect = effects[last].get();
            return static_cast<T*>(effect);
        }

        template<typename T, typename... Param>
        T* AddGenerator(Param&&... param) 
        {
            static_assert(std::is_base_of<AudioGenerator, T>::value, "AddGenerator parameter must derive from AudioGenerator");

            std::unique_ptr<AudioGenerator> ptr = std::make_unique<T>(std::forward<Param>(param)...);
            if (!ptr)
                return nullptr;

            generators.Add(std::move(ptr));
            size_t last = generators.GetCount() - 1;
            AudioGenerator* generator = generators[last].get();
            return static_cast<T*>(generator);
        }
    protected:
        void OnInitialize() override;
        void OnDestroy() override;
    private:
        ma_ex_audio_source *handle;
        Vector3 previousPosition;
        ConcurrentList<std::shared_ptr<AudioGenerator>> generators;
        ConcurrentList<std::shared_ptr<AudioEffect>> effects;
        std::atomic<bool> hasEnded;
        void Destroy();
        static void OnAudioEnded(void *pUserData, ma_sound *pSound);
        static void OnAudioLoaded(void *pUserData, ma_sound *pSound);
        static void OnAudioProcess(void* pUserData, ma_sound* pSound, float* pFramesOut, UInt64 frameCount, UInt32 channels);
        static void OnAudioRead(void *pUserData, void* pFramesOut, UInt64 frameCount, UInt32 channels);
    };
};
#endif