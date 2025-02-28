#include "PhaserEffect.hpp"
#include <vector>
#include <cmath>
#include "../Audio.hpp"

namespace GFX
{
    PhaserEffect::AllpassFilter::AllpassFilter() 
    {
        delayBuffer = std::vector<float>(44100, 0);
        writeIndex = 0;
        rate = 1.0f;
        feedback = 0.0f;
    }

    float PhaserEffect::AllpassFilter::Process(float input)
    {
        float delayedSample = delayBuffer[writeIndex];
        float output = -input + delayedSample;
        delayBuffer[writeIndex] = input + output * feedback;

        writeIndex = (writeIndex + 1) % delayBuffer.size();

        return output;
    }

    void PhaserEffect::AllpassFilter::SetRate(float newRate) 
    {
        rate = newRate;
        // Adjust the delay time based on the new rate
        delayBuffer.resize(static_cast<size_t>(44100 / rate), 0);
    }

    PhaserEffect::PhaserEffect()
    {
        numStages = 3;
        frequency = 1.0f;
        depth = 0.5f;
        feedback = 1.0f;

        for (int i = 0; i < numStages; ++i) 
        {
            allpassFilters.emplace_back();
        }
    }

    float PhaserEffect::Process(float input)
    {
        float output = input;

        for (int i = 0; i < numStages; ++i) 
        {
            output = allpassFilters[i].Process(output);
        }

        // Feedback loop
        feedbackBuffer = output * feedback + input * (1 - feedback);

        return output;
    }

    void PhaserEffect::OnProcess(AudioBuffer<float> pFrames, uint64_t frameCount, uint32_t channels)
    {
        for(size_t i = 0; i < pFrames.GetLength(); i++)
        {
            pFrames[i] = Process(pFrames[i]);
        }
    }

    void PhaserEffect::OnDestroy()
    {
        
    }

    void PhaserEffect::SetNumStages(int numStages)
    {
        this->numStages = numStages;

        allpassFilters.clear();

        for (int i = 0; i < numStages; ++i) 
        {
            allpassFilters.emplace_back();
        }
    }

    int PhaserEffect::GetNumStages() const
    {
        return numStages;
    }

    void PhaserEffect::SetFrequency(float frequency)
    {
        this->frequency = frequency;
        // Recalculate filter coefficients based on the new rate
        for (int i = 0; i < numStages; ++i) 
        {
            allpassFilters[i].SetRate(frequency);
        }
    }

    float PhaserEffect::GetFrequency() const
    {
        return frequency;
    }

    void PhaserEffect::SetDepth(float depth)
    {
        this->depth = depth;
    }

    float PhaserEffect::GetDepth() const
    {
        return depth;
    }

    void PhaserEffect::SetFeedback(float feedback)
    {
        this->feedback = feedback;
    }

    float PhaserEffect::GetFeedback() const
    {
        return feedback;
    }
}
