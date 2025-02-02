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
#include "../External/miniaudioex/miniaudioex.h"
#include <memory>
#include <type_traits> // For std::enable_if, std::is_base_of
#include <atomic>

namespace GFX
{
    class AudioSource;

    using AudioEndedCallback = std::function<void(AudioSource*)>;
    using AudioLoadedCallback = std::function<void(AudioSource*)>;
    using AudioProcessCallback = std::function<void(AudioSource*,AudioBuffer<float>, ma_uint64, ma_uint32)>;
    using AudioReadCallback = std::function<void(AudioSource*,AudioBuffer<float>, ma_uint64, ma_uint32)>;

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
        void SetCursor(ma_uint64 value);
        ma_uint64 GetCursor() const;
        ma_uint64 GetClipLength() const;
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
        T *AddEffect(Param... args)
        {
            static_assert(std::is_base_of<AudioEffect, T>::value, "AddEffect parameter must derive from AudioEffect");

            std::shared_ptr<AudioEffect> ptr = std::make_shared<T>(args...);
            if (!ptr)
                return nullptr;

            AudioEffect *effect = ptr.get();
            effects.Add(ptr);
            return static_cast<T*>(effect);
        }

        template<typename T, typename... Param>
        T *AddGenerator(Param... args)
        {
            static_assert(std::is_base_of<AudioGenerator, T>::value, "AddGenerator parameter must derive from AudioGenerator");

            std::shared_ptr<AudioGenerator> ptr = std::make_shared<T>(args...);
            if (!ptr)
                return nullptr;

            AudioGenerator *generator = ptr.get();
            generators.Add(ptr);
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
        static void OnAudioProcess(void* pUserData, ma_sound* pSound, float* pFramesOut, ma_uint64 frameCount, ma_uint32 channels);
        static void OnAudioRead(void *pUserData, void* pFramesOut, ma_uint64 frameCount, ma_uint32 channels);
    };
};
#endif