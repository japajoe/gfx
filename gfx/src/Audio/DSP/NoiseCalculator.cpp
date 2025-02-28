#include "NoiseCalculator.hpp"
#include "../../System/Random.hpp"

namespace GFX
{
	NoiseCalculator::NoiseCalculator()
	{
		this->type = NoiseType::White;
		previousValue = 0.0f;
		SetNoiseFunc();
	}

	NoiseCalculator::NoiseCalculator(NoiseType type)
	{
		this->type = type;
		previousValue = 0.0f;
		SetNoiseFunc();
	}

	void NoiseCalculator::SetType(NoiseType type)
	{
		this->type = type;
		previousValue = 0.0f;
		SetNoiseFunc();
	}

	NoiseType NoiseCalculator::GetType() const
	{
		return type;
	}

	float NoiseCalculator::GetValue()
	{
		return noiseFunc();
	}

	float NoiseCalculator::GetValue(float phase)
	{
		return noiseFunc();
	}

	void NoiseCalculator::SetNoiseFunc()
	{
		switch(type)
		{
			case NoiseType::Brown:
				noiseFunc = [this] () { return GetBrownNoise(); };
				break;
			case NoiseType::Pink:
				noiseFunc = [this] () { return GetPinkNoise(); };
				break;
			case NoiseType::White:
				noiseFunc = [this] () { return GetWhiteNoise(); };
				break;
		}
	}

	float NoiseCalculator::GetBrownNoise()
	{
		float newValue = static_cast<float>(Random::GetNextDouble() * 2 - 1);
		float output = (previousValue + newValue) / 2.0f;
		previousValue = newValue;
		return output;
	}

	float NoiseCalculator::GetPinkNoise()
	{
		float value1 = static_cast<float>(Random::GetNextDouble() * 2 - 1);
		float value2 = static_cast<float>(Random::GetNextDouble() * 2 - 1);
		float output = (value1 + value2) / 2.0f;
		return output;
	}

	float NoiseCalculator::GetWhiteNoise()
	{
		float output = static_cast<float>(Random::GetNextDouble() * 2 - 1);
		return output;
	}
}