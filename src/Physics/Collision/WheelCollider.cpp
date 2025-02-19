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
    WheelCollider::WheelCollider() : GameBehaviour()
    {
        radius = 0.5f;
        restLength = 0.5f;
        springTravel = 0.25f;
        springStiffness = 20000;
        damperStiffness = 10000;

        minLength = 0.0f;
        maxLength = 0.0f;
        lastLength = 0.0f;
        springLength = 0.5f;
        springVelocity = 0.0f;
        springForce = 0.0f;
        damperForce = 0.0f;

        center = Vector3(0.0f, 0.0f, 0.0f);
        suspensionForce = Vector3(0.0f, 0.0f, 0.0f);
        velocity = Vector3(0.0f, 0.0f, 0.0f);
        forceDirection = Vector3(0.0f, 0.0f, 0.0f);

        isEnabled = true;

        SetName("WheelCollider");
    }

    void WheelCollider::OnInitialize()
    {
        rigidBody = GetTransform()->GetRoot()->GetGameObject()->GetComponent<Rigidbody>();
    }

    void WheelCollider::OnFixedUpdate()
    {
        if(rigidBody == nullptr)
            return;

        if(!isEnabled)
            return;

        SetMinMaxLength();

        Transform *transform = GetTransform();
        Vector3 start = transform->GetPosition() +  GetWheelOffset();
        float length = maxLength + radius;

        RaycastHit hit;

        if (Physics::RayTest(start, -transform->GetUp(), length, hit))
        {
            if(hit.transform->GetRoot() != transform->GetRoot())
            {
                isGrounded = true;
                lastLength = springLength;
                springLength = glm::clamp((hit.distance - radius), -minLength, maxLength);
                springVelocity = (lastLength - springLength) / Time::GetDeltaTime();
                springForce = springStiffness * (restLength - springLength);
                damperForce = damperStiffness * springVelocity;
                suspensionForce = (springForce + damperForce) * transform->GetUp();
                //suspensionForce = (springForce + damperForce) * hit.normal;

                velocity = rigidBody->GetPointVelocity(hit.point);
                float fX = 0.0f * springForce;
                float fY = velocity.x * springForce;
                Vector3 force = suspensionForce + (fX * transform->GetForward()) + (fY * -transform->GetRight());

                auto point = Vector3(hit.point.x, hit.point.y + radius, hit.point.z);
                rigidBody->AddForceAtPoint(suspensionForce, point - transform->GetPosition());

                if(glm::abs(motorForce) > 0.0f)
                {
                    rigidBody->AddForceAtPoint(transform->GetForward() * motorForce, point - transform->GetPosition());
                }
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



        DrawDebugLines();
    }

    void WheelCollider::SetMinMaxLength()
    {
        minLength = restLength - springTravel;
        maxLength = restLength + springTravel;
    }

    void WheelCollider::DrawDebugLines()
    {
        Transform *transform = GetTransform();

        Vector3 offset = GetWheelOffset();
        Vector3 start = transform->GetPosition() + offset;
        Vector3 end = start + (-transform->GetUp() * (springLength + radius));

        Color color = isGrounded ? Color::Red() : Color::Green();
        Debug::DrawLine(start, end, color);
        
        Quaternion rotation = transform->GetRotation();
        DrawCircle(start, radius, rotation);

        end = start + (forceDirection * 2.0f);

        
        //Debug::DrawLine(start, end, color);
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

    Vector3 WheelCollider::GetWheelOffset() const
    {
        Transform *transform = GetTransform();
        Vector3 center = GetCenter();
        Vector3 offset =    center.x * transform->GetRight() +
                            center.y * transform->GetUp() +
                            center.z * transform->GetForward();
        return offset;
    }

    void WheelCollider::SetRadius(float radius)
    {
        this->radius = radius;
    }

    float WheelCollider::GetRadius() const
    {
        return radius;
    }

    void WheelCollider::SetRestLength(float restLength)
    {
        this->restLength = restLength;
    }

    float WheelCollider::GetRestLength() const
    {
        return restLength;
    }

    void WheelCollider::SetSpringTravel(float springTravel)
    {
        this->springTravel = springTravel;
    }

    float WheelCollider::GetSpringTravel() const
    {
        return springTravel;             
    }

    void WheelCollider::SetSpringStiffness(float springStiffness)
    {
        this->springStiffness = springStiffness;
    }

    float WheelCollider::GetSpringStiffness() const
    {
        return springStiffness;
    }

    void WheelCollider::SetDamperStiffness(float damperStiffness)
    {
        this->damperStiffness = damperStiffness;
    }

    float WheelCollider::GetDamperStiffness() const
    {
        return damperStiffness;
    }

    void WheelCollider::SetCenter(const Vector3 &center)
    {
        this->center = center;
    }
    
    Vector3 WheelCollider::GetCenter() const
    {
        return center;
    }

    bool WheelCollider::GetIsGrounded() const
    {
        return isGrounded;
    }

    void WheelCollider::SetIsEnabled(bool isEnabled)
    {
        this->isEnabled = isEnabled;    
    }

    bool WheelCollider::GetIsEnabled() const
    {
        return isEnabled;
    }

    void WheelCollider::SetMotorForce(float force)
    {
        motorForce = force;
    }

    float WheelCollider::GetMotorForce() const
    {
        return motorForce;
    }

    // WheelCollider::WheelCollider() : GameBehaviour()
    // {
    //     rigidbody = nullptr;
    //     restDistance = 0.75f;
    //     springStrength = 20000.0f;
    //     previousSpringLength = 0.0f;
    //     springDamper = 2000.0f;
    //     radius = 0.5f;
    //     center = Vector3(0, 0, 0);
    //     isGrounded = false;
    // }

    // void WheelCollider::OnInitialize()
    // {
    //     rigidbody = GetTransform()->GetRoot()->GetGameObject()->GetComponent<Rigidbody>();
    // }

    // void WheelCollider::OnFixedUpdate()
    // {
    //     if(rigidbody == nullptr)
    //         return;

    //     const Transform *transform = GetTransform();
    //     const Vector3 offset = GetWheelOffset();
    //     const Vector3 start = transform->GetPosition() + offset;
    //     const float length = restDistance + radius;
    //     const Vector3 end = start + (-transform->GetUp() * length);

    //     RaycastHit hit;

    //     if (Physics::RayTest(start, -transform->GetUp(), restDistance, hit))
    //     {
    //         if(hit.transform->GetRoot() != transform->GetRoot())
    //         {
    //             isGrounded = true;
    //             auto raycastDestination = hit.point;
    //             auto distance = Vector3f::Distance(hit.point, start);
    //             auto springLength = glm::clamp(distance - length, 0.0f, restDistance);
    //             auto springVelocity = (previousSpringLength - springLength) / Time::GetDeltaTime();
    //             previousSpringLength = springLength;
    //             printf("%f\n", springVelocity);
    //             auto springForce = springStrength + (restDistance - springLength);
    //             auto damperForce = springDamper * springVelocity;
    //             auto suspensionForce = (springForce + damperForce);
    //             auto point = Vector3(raycastDestination.x, raycastDestination.y + radius, raycastDestination.z);
    //             rigidbody->AddForceAtPoint(transform->GetUp() * suspensionForce, point - transform->GetPosition());
    //         }
    //         else
    //         {
    //             isGrounded = false;
    //         }
    //     }
    //     else
    //     {
    //         isGrounded = false;
    //     }

    //     Color color = isGrounded ? Color::Red() : Color::Green();
    //     Debug::DrawLine(start, end, color);
    // }

    // void WheelCollider::SetCenter(const Vector3 &center)
    // {
    //     this->center = center;
    // }
    
    // Vector3 WheelCollider::GetCenter() const
    // {
    //     return center;
    // }

    // bool WheelCollider::IsGrounded() const
    // {
    //     return isGrounded;
    // }

    // Vector3 WheelCollider::GetWheelOffset() const
    // {
    //     Transform *transform = GetTransform();
    //     Vector3 center = GetCenter();
    //     Vector3 offset =    center.x * transform->GetRight() +
    //                         center.y * transform->GetUp() +
    //                         center.z * transform->GetForward();
    //     return offset;
    // }

    // void WheelCollider::SetRadius(float value)
    // {
    //     radius = value;
    // }

    // float WheelCollider::GetRadius() const
    // {
    //     return radius;
    // }

    // void WheelCollider::SetSpringStrength(float value)
    // {
    //     springStrength = value;
    // }

    // float WheelCollider::GetSpringStrength() const
    // {
    //     return springStrength;
    // }

    // void WheelCollider::SetSpringDamping(float value)
    // {
    //     springDamper = value;
    // }

    // float WheelCollider::GetSpringDamping() const
    // {
    //     return springDamper;
    // }
}