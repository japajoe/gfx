#include "NoiseGenerator.hpp"
#include "../Audio.hpp"

namespace GFX
{
	NoiseGenerator::NoiseGenerator()
	{
		calculator = NoiseCalculator(NoiseType::White);
		wavetable = Wavetable(&calculator, 4096);
		frequency = 0.5f;
	}

	NoiseGenerator::NoiseGenerator(NoiseType type)
	{
		calculator = NoiseCalculator(type);
		wavetable = Wavetable(&calculator, 4096);
		frequency = 0.5f;
	}

	void NoiseGenerator::OnGenerate(AudioBuffer<float> pFrames, uint64_t frameCount, uint32_t channels)
	{
		float sample = 0;

		for(size_t i = 0; i < pFrames.GetLength(); i += channels)
		{
			sample = wavetable.GetValue(frequency, Audio::GetSampleRate());

			for(size_t j = 0; j < channels; j++)
			{
				pFrames[i+j] = sample;
			}
		}
	}

	void NoiseGenerator::OnDestroy()
	{

	}

	void NoiseGenerator::SetFrequency(float frequency)
	{
		this->frequency = frequency;
	}

	float NoiseGenerator::GetFrequency() const
	{
		return frequency;
	}
}