#ifndef GFX_WHEELCOLLIDER_HPP
#define GFX_WHEELCOLLIDER_HPP

#include "../../Core/GameBehaviour.hpp"
#include "../../System/Numerics/Vector2.hpp"
#include "../../System/Numerics/Vector3.hpp"
#include "../../System/Numerics/Vector4.hpp"
#include "../../System/Numerics/Quaternion.hpp"
#include "../RaycastHit.hpp"

namespace GFX
{
    class Rigidbody;
    
    class WheelCollider : public GameBehaviour
    {
    private:
        Rigidbody *rb;
        float restLength;
        float springTravel;
        float springStiffness;
        float damperStiffness;
        float wheelRadius;
        float motorTorque;
        float brakeTorque;
        float steerAngle;
        float slipRatio;
        Vector3 center;
        bool isEnabled;
        float minLength;
        float maxLength;
        float lastLength;
        float springLength;
        float springVelocity;
        float springForce;
        float damperForce;
        Vector3 suspensionForce;
        bool isGrounded;
        float rpm;
        Vector3 velocity;
        Vector3 hitPoint;
        bool debugLines;
        float angularVelocity;
        void UpdateSuspensionForce(const RaycastHit &hit);
        void UpdateMotorForce(const RaycastHit &hit);
        void CalculateRPM();
        void DrawDebugLines();
        void DrawCircle(const Vector3 &center, float radius, const Quaternion &rotation);
    protected:
        void OnInitialize() override;
        void OnUpdate() override;
        void OnLateUpdate() override;
        void OnFixedUpdate() override;
    public:
        Vector3 GetWheelOffset() const;
        Vector3 GetWheelPosition() const;
        void GetWorldPose(Vector3 &position, Quaternion &rotation);
        void SetRigidbody(Rigidbody *body);
        float GetRestLength() const;
        void SetRestLength(float length);
        float GetSpringTravel() const;
        void SetSpringTravel(float travel);
        float GetSpringStiffness() const;
        void SetSpringStiffness(float stiffness);
        float GetDamperStiffness() const;
        void SetDamperStiffness(float stiffness);
        float GetWheelRadius() const;
        void SetWheelRadius(float radius);
        float GetMotorTorque() const;
        void SetMotorTorque(float torque);
        float GetBrakeTorque() const;
        void SetBrakeTorque(float torque);
        float GetSteerAngle() const;
        void SetSteerAngle(float angle);
        const Vector3& GetCenter() const;
        void SetCenter(const Vector3& newCenter);
        bool IsEnabled() const;
        void SetEnabled(bool enabled);
        bool IsGrounded() const;
        void SetDebugLines(bool visible);
        void SetSlipRatio(float value);
        float GetSlipRatio() const;
        bool GetGroundHit(Vector3 &point);
    };
}

#endif