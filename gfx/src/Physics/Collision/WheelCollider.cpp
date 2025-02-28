#include "WheelCollider.hpp"
#include "../Physics.hpp"
#include "../Rigidbody.hpp"
#include "../../Core/Time.hpp"
#include "../../Core/Debug.hpp"
#include "../../Core/GameObject.hpp"
#include "../../Core/Transform.hpp"
#include "../../Core/Input.hpp"
#include "../../Graphics/Color.hpp"
#include "../../External/glm/glm.hpp"

namespace GFX
{
    void WheelCollider::OnInitialize()
    {
        rb = GetTransform()->GetRoot()->GetGameObject()->GetComponent<Rigidbody>();
        minLength = 0.0f;
        maxLength = 0.0f;
        lastLength = 0.0f;
        springLength = 0.0f;
        springVelocity = 0.0f;
        springForce = 0.0f;
        damperForce = 0.0f;
        suspensionForce = Vector3(0, 0, 0);
        slipRatio = 0.1f;
        
        restLength = 0.5f;
        springTravel = 0.2f;
        springStiffness = 2000.0f;
        damperStiffness = 250.0f;
        wheelRadius = 0.5f;
        steerAngle = 0.0f;
        center = Vector3(0, 0, 0);

        motorTorque = 0.0f;
        brakeTorque = 0.0f;

        isEnabled = true;
        rpm = 0;
        velocity = Vector3(0, 0, 0);
        debugLines = false;
    }

    void WheelCollider::OnUpdate()
    {
        if(!rb)
            return;

        if(!isEnabled)
            return;

        GetTransform()->SetLocalRotation(Quaternionf::Euler(Vector3f::UnitY() * glm::radians(steerAngle)));
    }

    void WheelCollider::OnLateUpdate()
    {
        if(!rb)
            return;

        if(!isEnabled)
            return;
        
        if(debugLines)
            DrawDebugLines();
    }

    void WheelCollider::OnFixedUpdate()
    {
        if(!rb)
            return;

        if(!isEnabled)
            return;

        minLength = restLength - springTravel;
        maxLength = restLength + springTravel;

        // minLength = 0;
        // maxLength = restLength;

        auto transform = GetTransform();
        Vector3 from = transform->GetPosition() + GetWheelOffset();
        Vector3 direction = Vector3f::Normalize(-transform->GetUp());
        RaycastHit hit;

        if (Physics::RayTest(from, direction, maxLength + wheelRadius, hit))
        {
            if(hit.transform->GetRoot() != GetTransform()->GetRoot())
            {
                UpdateSuspensionForce(hit);
                UpdateMotorForce(hit);
                isGrounded = true;
            }
            else
            {
                isGrounded = false;
            }
        }
        else
        {
            isGrounded = false;
        }

        CalculateRPM();

        if(glm::abs(motorTorque) > glm::epsilon<float>() || glm::abs(brakeTorque) > glm::epsilon<float>())
        {
            auto calculateMomentOfInertia = [this] () -> float {
                // Moment of inertia for a solid cylinder (wheel)
                const float mass = 40.0f;
                return 0.5f * mass * wheelRadius * wheelRadius;
            };

            float totalTorque = motorTorque - brakeTorque;
            float angularAcceleration = totalTorque / calculateMomentOfInertia();
            angularVelocity = angularAcceleration * Time::GetDeltaTime();
        }
        else
        {
            //angularVelocity = (transform->InverseTransformDirection(transform->GetPosition()).z * glm::length(rb->GetLinearVelocity())) / wheelRadius;
            angularVelocity = velocity.z / wheelRadius;
            angularVelocity *= -1.0f;
        }
    }

    void WheelCollider::UpdateSuspensionForce(const RaycastHit &hit)
    {
        auto transform = GetTransform();

        lastLength = springLength;
        springLength = hit.distance - wheelRadius;
        springLength = glm::clamp(springLength, minLength, maxLength);
        springVelocity = (lastLength - springLength) / Time::GetDeltaTime();
        springForce = springStiffness * (restLength - springLength);
        damperForce = damperStiffness * springVelocity;
        suspensionForce = (springForce + damperForce) * transform->GetUp();

        velocity = rb->GetPointVelocity(hit.point);
        velocity = transform->InverseTransformDirection(velocity) * glm::length(velocity);

        auto getWheelDirection = [&]() -> float {
            Vector3 carRight = rb->GetTransform()->GetRight();

            // Calculate the direction from the car to the wheel
            Vector3 directionToWheel = transform->GetPosition() - rb->GetTransform()->GetPosition();

            // Normalize the direction vector
            directionToWheel = glm::normalize(directionToWheel);

            // Use the dot product to determine the side
            float dotProduct = glm::dot(carRight, directionToWheel);

            if (dotProduct < 0)
                return -1.0f;
            else
                return 1.0f;
        };

        float fX = glm::abs(motorTorque ) > 0 ? glm::sign(motorTorque) * springForce : 0.0f;
        float fY = velocity.x * springForce * slipRatio;
        Vector3 frictionForce = (fX * transform->GetForward()) + (fY * -transform->GetRight());
        rb->AddForceAtPoint(suspensionForce + frictionForce, hit.point);
        hitPoint = hit.point;
    }

    void WheelCollider::UpdateMotorForce(const RaycastHit &hit)
    {
        auto transform = GetTransform();

        if(glm::abs(motorTorque) > 0.0f || glm::abs(brakeTorque) > 0.0f)
        {
            float totalTorque = motorTorque - brakeTorque;
            rb->AddForceAtPoint(transform->GetForward() * totalTorque, hit.point);
        }
    }

    void WheelCollider::CalculateRPM()
    {
        // Get the velocity of the wheel's contact point in world space
        Vector3 contactVelocity = rb->GetLinearVelocity();
        // The wheel speed in world space (only considering the forward speed in the x-direction)
        float wheelSpeed = glm::length(contactVelocity);

        const float wheelCircumference = 2 * glm::pi<float>() * wheelRadius;
        // Convert to RPM (rotations per minute)
        rpm = wheelSpeed / wheelCircumference * 60.0f * 0.01f;
    }

    Vector3 WheelCollider::GetWheelOffset() const
    {
        Transform *transform = GetTransform();
        Vector3 offset =    center.x * transform->GetRight() +
                            center.y * transform->GetUp() +
                            center.z * transform->GetForward();
        return offset;
    }

    Vector3 WheelCollider::GetWheelPosition() const
    {
        return GetTransform()->GetPosition() + GetWheelOffset();
    }

    void WheelCollider::GetWorldPose(Vector3 &position, Quaternion &rotation)
    {
        auto transform = GetTransform();
        //position = transform->GetPosition();

        position = GetWheelPosition() - transform->GetUp() * (wheelRadius - springLength);

        //float deltaRotation = rpm * Time::GetDeltaTime() * 360.0f / 60.0f;
        //rotation = transform->GetRotation() * Quaternionf::Euler(deltaRotation, 0.0f, 0.0f);
        rotation = transform->GetRotation() * Quaternionf::Euler(angularVelocity, 0.0f, 0.0f);
    }

    void WheelCollider::SetRigidbody(Rigidbody *body)
    {
        rb = body;
    }

    float WheelCollider::GetRestLength() const
    {
        return restLength;
    }

    void WheelCollider::SetRestLength(float length)
    {
        restLength = length;
    }

    float WheelCollider::GetSpringTravel() const
    {
        return springTravel;
    }

    void WheelCollider::SetSpringTravel(float travel)
    {
        springTravel = travel;
    }

    float WheelCollider::GetSpringStiffness() const
    {
        return springStiffness;
    }

    void WheelCollider::SetSpringStiffness(float stiffness)
    {
        springStiffness = stiffness;
    }

    float WheelCollider::GetDamperStiffness() const
    {
        return damperStiffness;
    }

    void WheelCollider::SetDamperStiffness(float stiffness)
    {
        damperStiffness = stiffness;
    }

    float WheelCollider::GetWheelRadius() const
    {
        return wheelRadius;
    }

    void WheelCollider::SetWheelRadius(float radius)
    {
        wheelRadius = radius;
    }

    float WheelCollider::GetMotorTorque() const
    {
        return motorTorque;
    }

    void WheelCollider::SetMotorTorque(float torque)
    {
        motorTorque = torque;
    }

    float WheelCollider::GetBrakeTorque() const
    {
        return brakeTorque;
    }

    void WheelCollider::SetBrakeTorque(float torque)
    {
        brakeTorque = torque;
    }

    float WheelCollider::GetSteerAngle() const
    {
        return steerAngle;
    }

    void WheelCollider::SetSteerAngle(float angle)
    {
        steerAngle = angle;
    }

    const Vector3& WheelCollider::GetCenter() const
    {
        return center;
    }

    void WheelCollider::SetCenter(const Vector3& newCenter)
    {
        center = newCenter;
    }

    bool WheelCollider::IsEnabled() const
    {
        return isEnabled;
    }

    void WheelCollider::SetEnabled(bool enabled)
    {
        isEnabled = enabled;
    }

    bool WheelCollider::IsGrounded() const
    {
        return isGrounded;
    }

    void WheelCollider::SetDebugLines(bool visible)
    {
        debugLines = visible;
    }

    void WheelCollider::SetSlipRatio(float value)
    {
        slipRatio = value;
    }

    float WheelCollider::GetSlipRatio() const
    {
        return slipRatio;
    }

    bool WheelCollider::GetGroundHit(Vector3 &point)
    {
        if(!isGrounded)
            return false;
        point = hitPoint;
        return true;
    }

    void WheelCollider::DrawDebugLines()
    {
        Transform *transform = GetTransform();

        Vector3 offset = GetWheelOffset();
        Vector3 start = transform->GetPosition() + offset;
        Vector3 end = start + (-transform->GetUp() * (springLength + wheelRadius));

        Color color = isGrounded ? Color::Red() : Color::Green();
        Debug::DrawLine(start, end, color);
        
        Quaternion rotation = transform->GetRotation();
        DrawCircle(start, wheelRadius, rotation);

        //end = start + (forceDirection * 2.0f);
        
        Debug::DrawLine(start, end, color);
    }

    void WheelCollider::DrawCircle(const Vector3 &center, float radius, const Quaternion &rotation)
    {
        int segments = 24;

        Quaternion additionalRotation = Quaternionf::FromAxisAngle(Vector3f::UnitY(), glm::radians(90.0f)); // add 90 degrees around the y-axis
        auto rot = rotation * additionalRotation; // apply the additional rotation        

        Vector3 xVector = rot * Vector3f::UnitX() * radius;
        Vector3 yVector = rot * Vector3f::UnitY() * radius;

        for (int i = 0; i < segments; i++)
        {
            float angle = i * glm::pi<float>() * 2 / segments;
            float nextAngle = (i + 1) * glm::pi<float>() * 2 / segments;

            Vector3 start = center + glm::cos(angle) * xVector + glm::sin(angle) * yVector;
            Vector3 end = center + glm::cos(nextAngle) * xVector + glm::sin(nextAngle) * yVector;

            Debug::DrawLine(start, end, Color::White());
        }
    }
}