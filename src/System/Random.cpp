#include "Random.hpp"
#include "../External/glm/glm.hpp"
#include <random>

namespace GFX
{
    std::random_device rdev;
    std::mt19937 rgen(rdev());
    std::uniform_int_distribution<int> dist_byte(0, 255);
    std::uniform_real_distribution<double> dist_double(0.0, 1.0);

    void Random::Seed(uint32_t seed)
    {
        rgen.seed(seed);
    }

    double Random::GetNextDouble()
    {
        return dist_double(rgen);
    }

    uint8_t Random::GetNextByte()
    {
        return static_cast<uint8_t>(dist_byte(rgen));
    }

    double Random::Range(double min, double max)
    {
        double x = GetNextDouble() * (max - min);
        return min + x;
    }

    void Random::InsideUnitSphere(float &x, float &y, float &z)
    {
        float theta = static_cast<float>(2 * glm::pi<float>() * GetNextDouble());
        x = glm::sin(theta);
        y = glm::cos(theta);
        z = glm::sin(theta) * glm::sin(theta);
    }
}