#include "MouseOrbit.hpp"
#include "GameObject.hpp"
#include "Camera.hpp"
#include "Transform.hpp"
#include "Time.hpp"
#include "Input.hpp"
#include "../Physics/Physics.hpp"
#include "../External/glm/glm.hpp"

namespace GFX
{
    void MouseOrbit::OnInitialize()
    {
        target = nullptr;

        autoRotate = false;
        rotationSpeed = 8.0f;
        startRotation = 0.0f;
        rotationSmoothing = 20.0f;
        rotationSensitivity = 0.005f * 200.0f;
        rotationLimit = MinMaxConstraint(-90.0f, 90);
        zAxisDistance = 20;
        cameraZoomRangeZAxis = MinMaxConstraint(3.0f, 60.0f);
        zoomSmoothness = 5.0f;
        zoomSensitivity = 10.0f;

        xVelocity = 0.0f;
        yVelocity = 0.0f;
        xRotationAxis = 0.0f;
        yRotationAxis = 0.0f;
        zoomVelocityZAxis = 0.0f;
        canRotate = true;
        canControl = true;


        camera = GetGameObject()->GetComponent<Camera>();

        rotationLimit.min += 15.0f;
        rotationLimit.max -= 15.0f;

        //Sets the camera's rotation along the y axis.
        //The reason we're dividing by rotationSpeed is because we'll be multiplying by rotationSpeed in LateUpdate.
        //So we're just accounting for that at start.
        xRotationAxis = startRotation / rotationSpeed;

        Vector3 angles = Quaternionf::ToEulerAngles(GetTransform()->GetRotation());
        xRotationAxis = -180.0f;
        yRotationAxis = -180.0f;

        SetName("MouseOrbit");
    }

    void MouseOrbit::OnUpdate()
    {
        if(!canControl)
            return;
    }

    void MouseOrbit::OnLateUpdate()
    {                
        //If auto rotation is enabled, just increment the xVelocity value by the rotationSensitivity.
        //As that value's tied to the camera's rotation, it'll rotate automatically.
        if (autoRotate)
        {
            xVelocity += rotationSensitivity * Time::GetDeltaTime();
        }
        
        if (target != nullptr)
        {
            Quaternion rotation;
            Vector3 position;
            float deltaTime = Time::GetDeltaTime();

            if (canRotate && canControl)
            {
                auto mouseDelta = Input::GetMouseDelta();
                
                float deltaX = mouseDelta.x * Time::GetDeltaTime();
                float deltaY = mouseDelta.y * Time::GetDeltaTime();

                xVelocity -= deltaX * rotationSensitivity;
                yVelocity -= deltaY * rotationSensitivity * 8;
            }

            xRotationAxis -= xVelocity;
            yRotationAxis += yVelocity;

            //Clamp the rotation along the y-axis between the limits we set. 
            //Limits of 360 or -360 on any axis will allow the camera to rotate unrestricted
            yRotationAxis = ClampAngleBetweenMinAndMax(yRotationAxis, rotationLimit.min, rotationLimit.max);

            rotation = Euler(yRotationAxis, -xRotationAxis * rotationSpeed, 0);
            position = rotation * Vector3(0.0f, 0.0f, zAxisDistance) + target->GetPosition();

            Transform *transform = GetTransform();

            transform->SetRotation(rotation);
            transform->SetPosition(position);

            xVelocity = glm::lerp(xVelocity, 0.0f, deltaTime * rotationSmoothing);
            yVelocity = glm::lerp(yVelocity, 0.0f, deltaTime * rotationSmoothing);
        }
    }

    void MouseOrbit::OnToggledInput(bool enabled)
    {
        canControl = enabled;
    }

    void MouseOrbit::ToggleCanControl(bool enabled)
    {
        canControl = enabled;
    }

    //Prevents the camera from locking after rotating a certain amount if the rotation limits are set to 360 degrees.
    float MouseOrbit::ClampAngleBetweenMinAndMax(float angle, float min, float max)
    {
        if (angle < -360)
        {
            angle += 360;
        }
        if (angle > 360)
        {
            angle -= 360;
        }
        return glm::clamp(angle, min, max);
    }

    Quaternion MouseOrbit::Euler(float x, float y, float z)
    {
        return Quaternionf::Euler(glm::radians(x), glm::radians(y), glm::radians(z));
    }

    void MouseOrbit::SetTarget(Transform *target)
    {
        this->target = target;
    }
}