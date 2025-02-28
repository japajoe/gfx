#ifndef GFX_NOISECALCULATOR_HPP
#define GFX_NOISECALCULATOR_HPP

#include "Wavetable.hpp"
#include <functional>

namespace GFX
{
    enum class NoiseType
    {
        Brown,
        Pink,
        White
    };

	using NoiseFunc = std::function<float()>;

    class NoiseCalculator : public IWaveCalculator
    {
	private:
        NoiseType type;
        NoiseFunc noiseFunc;
        float previousValue;
		void SetNoiseFunc();
		float GetBrownNoise();
		float GetPinkNoise();
		float GetWhiteNoise();
	public:
        NoiseCalculator();
        NoiseCalculator(NoiseType type);
		void SetType(NoiseType type);
        NoiseType GetType() const;
        float GetValue();
        float GetValue(float phase) override;
    };
}

#endif