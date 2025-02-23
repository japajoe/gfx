#ifndef GFX_OSCILLATOR_HPP
#define GFX_OSCILLATOR_HPP

namespace GFX
{
    enum class WaveType
    {
        Sine,
        Square,
        Triangle,
        Saw
    };

    class Oscillator
    {
    private:
        typedef float (*WaveFunction)(float phase);
        WaveFunction waveFunc;
        WaveType type;
        float frequency;
        float amplitude;
        float phase;
        float phaseIncrement;
        void SetWaveFunction();
        void SetPhaseIncrement();
    public:
        Oscillator();
        Oscillator(WaveType type, float frequency, float amplitude);
        void Reset();
        float GetValue();
        float GetValueAtPhase(float phase);
        float GetModulatedValue(float phase);
        void SetType(WaveType type);
        WaveType GetType() const;
        void SetFrequency(float frequency);
        float GetFrequency() const;
        void SetAmplitude(float amplitude);
        float GetAmplitude() const;    
        static float GetSawSample(float phase);
        static float GetSineSample(float phase);
        static float GetSquareSample(float phase);
        static float GetTriangleSample(float phase);

        bool operator==(const Oscillator &other)
        {
            return &other == this;
        }
    };
}

#endif