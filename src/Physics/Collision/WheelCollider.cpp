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
        
        restLength = 0.5f;
        springTravel = 0.2f;
        springStiffness = 2000.0f;
        damperStiffness = 250.0f;
        wheelRadius = 0.5f;
        steerAngle = 0.0f;
        center = Vector3(0, 0, 0);
    }

    void WheelCollider::OnUpdate()
    {
        if(!rb)
            return;

        auto transform = GetTransform();
        Vector3 currentEuler = Quaternionf::ToEulerAngles(transform->GetLocalRotation());
        auto rotation = Quaternion(Vector3(currentEuler.x, glm::radians(steerAngle), currentEuler.z));
        transform->SetLocalRotation(rotation);
    }

    void WheelCollider::OnFixedUpdate()
    {
        if(!rb)
            return;

        minLength = restLength - springTravel;
        maxLength = restLength + springTravel;

        auto transform = GetTransform();
        Vector3 from = transform->GetPosition() + GetWheelOffset();
        Vector3 direction = Vector3f::Normalize(-transform->GetUp());
        RaycastHit hit;

        if (Physics::RayTest(from, direction, maxLength + wheelRadius, hit))
        {
            if(hit.transform->GetRoot() != GetTransform()->GetRoot())
            {
                lastLength = springLength;
                springLength = hit.distance - wheelRadius;
                springLength = glm::clamp(springLength, minLength, maxLength);
                springVelocity = (lastLength - springLength) / Time::GetDeltaTime();
                springForce = springStiffness * (restLength - springLength);
                damperForce = damperStiffness * springVelocity;
                suspensionForce = (springForce + damperForce) * transform->GetUp();

                auto wheelVelocity = transform->InverseTransformDirection(rb->GetPointVelocity(hit.point));
                float fX = glm::abs(motorTorque ) > 0 ? glm::sign(motorTorque) * springForce : 0.0f;
                float fY = wheelVelocity.x * springForce;

                Vector3 frictionForce = (fX * transform->GetForward()) + (fY * -transform->GetRight());

                rb->AddForceAtPoint(suspensionForce + frictionForce, hit.point);
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

        if(glm::abs(motorTorque) > 0.0f)
        {
            rb->AddForceAtPoint(transform->GetForward() * motorTorque, hit.point);
        }

        DrawDebugLines();
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