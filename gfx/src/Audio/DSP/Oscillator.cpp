#include "Oscillator.hpp"
#include "../Audio.hpp"
#include "../../../libs/miniaudioex/include/miniaudioex.h"
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265359
#endif

#ifndef TAU
#define TAU (M_PI * 2)
#endif

static MA_INLINE float absf(float x)
{
    return x > 0.0f ? x : -x;
}

static MA_INLINE int sign(float num) 
{
    if (num > 0)
        return 1;
    else if (num < 0)
        return -1;
    else
        return 0;
}

namespace GFX
{
    Oscillator::Oscillator()
    {
        this->type = WaveType::Sine;
        this->phase = 0.0f;
        this->frequency = 440.0f;
        this->amplitude = 1.0f;
        SetPhaseIncrement();
        SetWaveFunction();
    }

    Oscillator::Oscillator(WaveType type, float frequency, float amplitude)
    {
        this->type = type;
        this->phase = 0.0f;
        this->frequency = frequency;
        this->amplitude = amplitude;
        SetPhaseIncrement();
        SetWaveFunction();
    }

    void Oscillator::SetWaveFunction()
    {
        switch(type)
        {
            case WaveType::Saw:
                waveFunc = GetSawSample;
                break;
            case WaveType::Sine:
                waveFunc = GetSineSample;
                break;
            case WaveType::Square:
                waveFunc = GetSquareSample;
                break;
            case WaveType::Triangle:
                waveFunc = GetTriangleSample;
                break;
        }
    }

    void Oscillator::SetPhaseIncrement()
    {
        phaseIncrement = TAU * frequency / Audio::GetSampleRate();
    }

    void Oscillator::Reset()
    {
        phase = 0;
    }

    float Oscillator::GetValue()
    {
        float result = waveFunc(phase);
        phase += phaseIncrement;
        phase = fmodf(phase, TAU);
        return result * amplitude;
    }

    float Oscillator::GetValueAtPhase(float phase)
    {
        return waveFunc(phase) * amplitude;
    }

    float Oscillator::GetModulatedValue(float phase)
    {
        float result = waveFunc(this->phase + phase);
        this->phase += phaseIncrement;
        this->phase = fmodf(this->phase, TAU);
        return result * amplitude;
    }

    void Oscillator::SetType(WaveType type)
    {
        this->type = type;
        SetWaveFunction();
    }

    WaveType Oscillator::GetType() const
    {
        return type;
    }

    void Oscillator::SetFrequency(float frequency)
    {
        this->frequency = frequency;
        SetPhaseIncrement();
    }

    float Oscillator::GetFrequency() const
    {
        return frequency;
    }

    void Oscillator::SetAmplitude(float amplitude)
    {
        this->amplitude = amplitude;
    }

    float Oscillator::GetAmplitude() const
    {
        return amplitude;
    }


    float Oscillator::GetSawSample(float phase)
    {
        phase = phase / TAU;
        return 2.0f * phase - 1.0f;
    }

    float Oscillator::GetSineSample(float phase)
    {
        return sinf(phase);
    }

    float Oscillator::GetSquareSample(float phase)
    {
        return sign(sinf(phase));
    }

    float Oscillator::GetTriangleSample(float phase)
    {
        phase = phase / TAU;
        return (2.0f * absf(2.0f * (phase - 0.5f)) - 1.0f);
    }
}