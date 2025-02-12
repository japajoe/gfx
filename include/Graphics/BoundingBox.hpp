#ifndef GFX_BOUNDINGBOX_HPP
#define GFX_BOUNDINGBOX_HPP

#include "../System/Numerics/Vector3.hpp"
#include "../System/Numerics/Matrix4.hpp"
#include "../Physics/RaycastHit.hpp"

namespace GFX
{
    class BoundingBox
    {
    private:
        Vector3 min;
        Vector3 max;
        Vector3 center;
        Vector3 extents;
        bool hasPoint;
    public:
        BoundingBox();
        BoundingBox(const Vector3 &min, const Vector3 &max);
        void Grow(const Vector3 &point);
        void Grow(const Vector3 &min, const Vector3 &max);
        Vector3 GetMin() const;
        Vector3 GetMax() const;
        Vector3 GetCenter() const;
        Vector3 GetExtents() const;
        Vector3 GetSize() const;
        bool HasPoint() const;
        void Clear();
        bool Intersects(const Ray &ray, float &distance) const;
        bool ContainSphere(const Vector3 &center, float radius, float &distance) const;
        void Transform(const Matrix4 &transformation);
    };
}

#endif