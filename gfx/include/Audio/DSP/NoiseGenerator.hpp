#ifndef GFX_NOISEGENERATOR_HPP
#define GFX_NOISEGENERATOR_HPP

#include "AudioGenerator.hpp"
#include "Wavetable.hpp"
#include "NoiseCalculator.hpp"

namespace GFX
{
    class NoiseGenerator : public AudioGenerator
    {
    private:
        Wavetable wavetable;
        NoiseCalculator calculator;
        float frequency;
    public:
        NoiseGenerator();
        NoiseGenerator(NoiseType type);
        void OnGenerate(AudioBuffer<float> pFrames, uint64_t frameCount, uint32_t channels) override;
        void OnDestroy() override;
        void SetFrequency(float frequency);
        float GetFrequency() const;
    };
}

#endif