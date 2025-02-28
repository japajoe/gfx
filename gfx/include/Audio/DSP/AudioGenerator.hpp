#ifndef GFX_AUDIOGENERATOR_HPP
#define GFX_AUDIOGENERATOR_HPP

#include <cstdint>
#include "../AudioBuffer.hpp"

namespace GFX
{
    class AudioGenerator
    {
    public:
        virtual ~AudioGenerator() {}
        virtual void OnGenerate(AudioBuffer<float> pFrames, uint64_t frameCount, uint32_t channels) {}
        virtual void OnDestroy() {}
    };
}

#endif