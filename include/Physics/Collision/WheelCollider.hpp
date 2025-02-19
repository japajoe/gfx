#ifndef GFX_WHEELCOLLIDER_HPP
#define GFX_WHEELCOLLIDER_HPP

#include "../../Core/GameBehaviour.hpp"
#include "../../System/Numerics/Vector2.hpp"
#include "../../System/Numerics/Vector3.hpp"
#include "../../System/Numerics/Vector4.hpp"
#include "../../System/Numerics/Quaternion.hpp"

namespace GFX
{
    class Rigidbody;
    
    class WheelCollider : public GameBehaviour
    {
    private:
        Rigidbody *rb;
        float minLength;
        float maxLength;
        float lastLength;
        float springLength;
        float springVelocity;
        float springForce;
        float damperForce;
        Vector3 suspensionForce;
        bool isGrounded;
        void DrawDebugLines();
        void DrawCircle(const Vector3 &center, float radius, const Quaternion &rotation);
    protected:
        void OnInitialize() override;
        void OnUpdate() override;
        void OnFixedUpdate() override;
    public:
        float restLength;
        float springTravel;
        float springStiffness;
        float damperStiffness;
        float wheelRadius;
        float motorTorque;
        float steerAngle;
        Vector3 center;
        Vector3 GetWheelOffset() const;
        Vector3 GetWheelPosition() const;
    };
}

#endif