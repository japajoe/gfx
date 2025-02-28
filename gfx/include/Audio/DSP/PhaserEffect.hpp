#ifndef GFX_PHASEREFFECT_HPP
#define GFX_PHASEREFFECT_HPP

#include "AudioEffect.hpp"
#include <vector>

namespace GFX
{
    class PhaserEffect : public AudioEffect
    {
    private:
        class AllpassFilter 
        {
        public:
            AllpassFilter();
            float Process(float input);
            void SetRate(float newRate);
        private:
            std::vector<float> delayBuffer;
            size_t writeIndex;
            float rate;
            float feedback;
        };

        int numStages;
        float frequency;
        float depth;
        float feedback;
        float feedbackBuffer;
        std::vector<AllpassFilter> allpassFilters;
        float Process(float input);
    public:
        PhaserEffect();
        void OnProcess(AudioBuffer<float> pFrames, uint64_t frameCount, uint32_t channels) override;
        void OnDestroy() override;
        void SetNumStages(int numStages);
        int GetNumStages() const;
        void SetFrequency(float frequency);
        float GetFrequency() const;
        void SetDepth(float depth);
        float GetDepth() const;
        void SetFeedback(float feedback);
        float GetFeedback() const;
    };
}

#endif
