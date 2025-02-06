#include "BoundingBox.hpp"
#include "../System/Numerics/Vector4.hpp"
#include <limits>

namespace GFX
{
    BoundingBox::BoundingBox()
    {
        Clear();
    }

    BoundingBox::BoundingBox(const Vector3 &min, const Vector3 &max)
    {
        Clear();
        this->min = min;
        this->max = max;
        center = (min + max) * 0.5f;
        extents = max - center;
        hasPoint = true;
    }

    void BoundingBox::Grow(const Vector3 &point)
    {
        min = Vector3f::Min(min, point);
        max = Vector3f::Max(max, point);
        center = (min + max) * 0.5f;
        extents = max - center;
        hasPoint = true;
    }

    void BoundingBox::Grow(const Vector3 &min, const Vector3 &max)
    {
        if (hasPoint)
        {
            this->min.x = min.x < this->min.x ? min.x : this->min.x;
            this->min.y = min.y < this->min.y ? min.y : this->min.y;
            this->min.z = min.z < this->min.z ? min.z : this->min.z;
            this->max.x = max.x > this->max.x ? max.x : this->max.x;
            this->max.y = max.y > this->max.y ? max.y : this->max.y;
            this->max.z = max.z > this->max.z ? max.z : this->max.z;
        }
        else
        {
            hasPoint = true;
            this->min = min;
            this->max = max;
        }
    }

    void BoundingBox::Clear()
    {
        min = Vector3f::One() *  std::numeric_limits<float>::infinity();
        max = Vector3f::One() * - std::numeric_limits<float>::infinity();
        center = (min + max) * 0.5f;
        extents = max - center;
        hasPoint = false;
    }

    Vector3 BoundingBox::GetMin() const
    {
        return min;
    }

    Vector3 BoundingBox::GetMax() const
    {
        return max;
    }

    Vector3 BoundingBox::GetCenter() const
    {
        return center;
    }

    Vector3 BoundingBox::GetExtents() const
    {
        return extents;
    }

    Vector3 BoundingBox::GetSize() const
    {
        return max - min;
    }

    bool BoundingBox::HasPoint() const
    {
        return hasPoint;
    }

    static constexpr float ZeroTolerance = 1e-6f;
    static constexpr float FloatMaxValue = 3.4028235E38F;

    static bool IsZero(float value)
    {
        return glm::abs(value) < ZeroTolerance;
    }

    bool BoundingBox::Intersects(const Ray &ray, float &distance) const
    {
        distance = 0.0f;
        float tmax = FloatMaxValue;

        if (IsZero(ray.direction.x))
        {
            if (ray.origin.x < min.x || ray.origin.x > max.x)
            {
                distance = 0.0f;
                return false;
            }
        }
        else
        {
            const float inverse = 1.0f / ray.direction.x;
            float t1 = (min.x - ray.origin.x) * inverse;
            float t2 = (max.x - ray.origin.x) * inverse;

            if (t1 > t2)
            {
                const float temp = t1;
                t1 = t2;
                t2 = temp;
            }

            distance = glm::max(t1, distance);
            tmax = glm::min(t2, tmax);

            if (distance > tmax)
            {
                distance = 0.0f;
                return false;
            }
        }

        if (IsZero(ray.direction.y))
        {
            if (ray.origin.y < min.y || ray.origin.y > max.y)
            {
                distance = 0.0f;
                return false;
            }
        }
        else
        {
            const float inverse = 1.0f / ray.direction.y;
            float t1 = (min.y - ray.origin.y) * inverse;
            float t2 = (max.y - ray.origin.y) * inverse;

            if (t1 > t2)
            {
                const float temp = t1;
                t1 = t2;
                t2 = temp;
            }

            distance = glm::max(t1, distance);
            tmax = glm::min(t2, tmax);

            if (distance > tmax)
            {
                distance = 0.0f;
                return false;
            }
        }

        if (IsZero(ray.direction.z))
        {
            if (ray.origin.z < min.z || ray.origin.z > max.z)
            {
                distance = 0.0f;
                return false;
            }
        }
        else
        {
            const float inverse = 1.0f / ray.direction.z;
            float t1 = (min.z - ray.origin.z) * inverse;
            float t2 = (max.z - ray.origin.z) * inverse;

            if (t1 > t2)
            {
                const float temp = t1;
                t1 = t2;
                t2 = temp;
            }

            distance = glm::max(t1, distance);
            tmax = glm::min(t2, tmax);

            if (distance > tmax)
            {
                distance = 0.0f;
                return false;
            }
        }

        return true;
    }

    bool BoundingBox::ContainSphere(const Vector3 &center, float radius, float &distance) const
    {
        return false;
    }

    void BoundingBox::Transform(const Matrix4 &transformation)
    {
        min = transformation * Vector4(min, 1.0f);
        max = transformation * Vector4(max, 1.0f);
        center = (min + max) * 0.5f;
        extents = max - center;
    }
}