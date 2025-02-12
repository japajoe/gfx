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
        float radius;
        float restLength;
        float springTravel;
        float springStiffness;
        float damperStiffness;
        Vector3 center;
        float minLength;
        float maxLength;
        float lastLength;
        float springLength;
        float springVelocity;
        float springForce;
        float damperForce;
        Vector3 suspensionForce;
        Vector3 velocity; //Velocity of the wheel in local space
        Rigidbody *rigidBody;
        bool isGrounded;
        Vector3 forceDirection;
        bool isEnabled;
        void SetMinMaxLength();
        void DrawDebugLines();
        void DrawCircle(const Vector3 &center, float radius, const Quaternion &rotation);
        Vector3 GetWheelOffset() const;
    protected:
        void OnInitialize() override;
        void OnFixedUpdate() override;
    public:
        WheelCollider();
        void SetRadius(float radius);
        float GetRadius() const;
        void SetRestLength(float restLength);
        float GetRestLength() const;
        void SetSpringTravel(float springTravel);
        float GetSpringTravel() const;
        void SetSpringStiffness(float springStiffness);
        float GetSpringStiffness() const;
        void SetDamperStiffness(float damperStiffness);
        float GetDamperStiffness() const;
        void SetCenter(const Vector3 &center);
        Vector3 GetCenter() const;
        bool GetIsGrounded() const;
        void SetIsEnabled(bool isEnabled);
        bool GetIsEnabled() const;
    };
}

#endif