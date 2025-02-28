#ifndef GFX_WAVETABLE_HPP
#define GFX_WAVETABLE_HPP

#include <vector>
#include <cstdint>
#include <cstdlib>

namespace GFX
{
    class IWaveCalculator
    {
	public:
        virtual float GetValue(float phase) { return 0.0f; }
    };

    class Wavetable
    {
	private:
        std::vector<float> data;
        int length;
        int index;
        float phase;
        float phaseIncrement;
		float Interpolate(float value1, float value2, float t);
	public:
        Wavetable();
        Wavetable(IWaveCalculator *calculator, size_t length);
        Wavetable(const std::vector<float> &data);
        float GetValue(float frequency, float sampleRate);
        float GetValueAtPhase(float phase);
    };
}

#endif