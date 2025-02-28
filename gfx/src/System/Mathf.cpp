#include "Mathf.hpp"
#include "Numerics/Noise.hpp"
#include <algorithm>
#include "../External/glm/glm.hpp"

namespace GFX
{
    float Mathf::Exp(float power)
    {
        return glm::exp(power);
    }

    float Mathf::Min(float a, float b)
    {
        return glm::min(a, b);
    }

    float Mathf::Max(float a, float b)
    {
        return glm::max(a, b);
    }

    float Mathf::Abs(float value)
    {
        return glm::abs(value);
    }

    float Mathf::Clamp(float value, float min, float max)
    {
		return glm::clamp(value, min, max);
    }

    float Mathf::Clamp01(float value)
    {
        return glm::max(0.0f, glm::min(value, 1.0f));
    }

    int32_t Mathf::FloorToInt(float value)
    {
        return static_cast<int32_t>(glm::floor(value));
    }

    float Mathf::Floor(float value)
    {
        return glm::floor(value);
    }

    float Mathf::InverseLerp(float start, float end, float value)
    {
        return (value - start) / (end - start);
    }

    float Mathf::InverseLerpClamped(float start, float end, float value)
    {
        return glm::clamp((value - start) / (end - start), 0.0f, 1.0f);
    }

    float Mathf::Lerp(float start, float end, float percentage)
    {
        return start + ((end-start) * percentage);	
    }

    float Mathf::Sin(float x)
    {
        return glm::sin(x);
    }

    float Mathf::Cos(float x)
    {
        return glm::cos(x);
    }

    float Mathf::Sqrt(float x)
    {
        return glm::sqrt(x);
    }

    float Mathf::Pow(float x, float y)
    {
        return glm::pow(x, y);
    }

    float Mathf::Atan2(float x, float y)
    {
        return glm::atan2(x, y);
    }

    float Mathf::Acos(float x)
    {
        return glm::acos(x);
    }

    float Mathf::Sign(float value)
    {
        return value >= 0.0 ? 1.0f : -1.0f;
    }

    float Mathf::Tan(float value)
    {
        return glm::tan(value);
    }

    float Mathf::ModF(float x, float y)
    {
        return glm::mod(x, y);
    }

    float Mathf::DegreesToRadians(float degrees)
    {
        return Mathf::Deg2Rad * degrees;
    }

    float Mathf::RadiansToDegrees(float radians)
    {
        return Mathf::Rad2Deg * radians;
    }

    float Mathf::ClampPeriodicRange(float a, float min, float max)
    {
        float result = a;
        float range = (max - min);
        while(result >= max)
            result -= range;
        while(result < min)
            result += range;
        return result;
    }

    bool Mathf::IsNan(float value)
    {
        return glm::isnan(value);
    }

    bool Mathf::IsZero(float value)
    {
        return Abs(value) < ZeroTolerance;
    }

    float Mathf::PerlinNoise(float x, float y)
    {
        return Noise::GetSample(x, y);
    }
}