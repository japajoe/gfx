#include "FMGenerator.hpp"

namespace GFX
{
    FMGenerator::FMGenerator()
    {
        carrier = Oscillator(WaveType::Sine, 440.0f, 1.0f);
    }

    FMGenerator::FMGenerator(WaveType type, float frequency, float amplitude)
    {
        carrier = Oscillator(type, frequency, amplitude);
    }

    int FMGenerator::GetCount() const
    {
        return operators.GetCount();
    }

    Oscillator *FMGenerator::operator[](int index)
    {
        if (index >= operators.GetCount())
            return nullptr;
        return &operators[index];
    }

    Oscillator *FMGenerator::GetCarrier()
    {
        return &carrier;    
    }

    Oscillator *FMGenerator::GetOperator(size_t index)
    {
        if(index >= operators.GetCount())
            return nullptr;
        return &operators[index];
    }

    void FMGenerator::Reset()
    {
        carrier.Reset();
        for(int i = 0; i < operators.GetCount(); i++)
        {
            operators[i].Reset();
        }
    }

    void FMGenerator::AddOperator(WaveType type, float frequency, float depth)
    {
        operators.Add(Oscillator(type, frequency, depth));
    }

    void FMGenerator::RemoveOperator(int index)
    {
        if(index >= 0 && index < operators.GetCount())
        {
            auto &target = operators[index];
            operators.Remove(target);
        }
    }

    void FMGenerator::OnGenerate(AudioBuffer<float> pFrames, uint64_t frameCount, uint32_t channels)
    {
        uint64_t samples = frameCount * channels;
        float sample = 0;

        for(uint64_t i = 0; i < samples; i+=channels)
        {
            sample = GetModulatedSample();

            for(uint32_t j = 0; j < channels; j++)
            {
                pFrames[i+j] = sample;
            }
        }
    }

    float FMGenerator::GetModulatedSample()
    {
        float modulationSum = 0.0f;

        for (int i = 0; i < operators.GetCount(); i++)
        {
            modulationSum += operators[i].GetValue();
        }

        return carrier.GetModulatedValue(modulationSum);
    }
}