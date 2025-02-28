#ifndef GFX_MATHF_HPP
#define GFX_MATHF_HPP

#include <cstdint>
#include <limits>
#include <cfloat>

namespace GFX
{
    class Mathf
    {
    public:
        static constexpr float PI = 3.14159265359f;
        static constexpr float TAU = 2.0f * PI;
        static constexpr float Rad2Deg = 180.0f / PI;
        static constexpr float Deg2Rad = PI / 180.0f;
        static constexpr float FloatMinValue = -FLT_MAX;
        static constexpr float FloatMaxValue = FLT_MAX;
        static constexpr float Infinity = std::numeric_limits<float>::infinity();
        static constexpr float Epsilon = std::numeric_limits<float>::epsilon();
        static constexpr float ZeroTolerance = 1e-6f;

        static float Exp(float power);
        static float Min(float a, float b);
        static float Max(float a, float b);        
        static float Abs(float value);
        static float Clamp(float value, float min, float max);
        static float Clamp01(float value);
        static int32_t FloorToInt(float value);
        static float Floor(float value);
        static float InverseLerp(float start, float end, float value);
        static float InverseLerpClamped(float start, float end, float value);
        static float Lerp(float start, float end, float percentage);
        static float Sin(float x);
        static float Cos(float x);
        static float Sqrt(float x);
        static float Pow(float x, float y);
        static float Atan2(float x, float y);
        static float Acos(float x);
        static float Sign(float value);
        static float Tan(float value);
        static float ModF(float x, float y);
        static float DegreesToRadians(float degrees);
        static float RadiansToDegrees(float radians);
        static float ClampPeriodicRange(float a, float min, float max);
        static bool IsNan(float value);
        static bool IsZero(float value);
        static float PerlinNoise(float x, float y);
    };
}

#endif