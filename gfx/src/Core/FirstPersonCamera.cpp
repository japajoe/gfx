#include "FirstPersonCamera.hpp"
#include "Input.hpp"
#include "Time.hpp"
#include "Transform.hpp"
#include "../External/glm/glm.hpp"
#include <limits>

namespace GFX
{
    void FirstPersonCamera::OnInitialize()
    {
        currentRotation.x = 0;
        currentRotation.y = 0;
        zoomSpeed = 2000.0f;
        speed = 50.0f;
        rotationSpeed = 0.012f;
        isInitialized = false;
        isControllable = true;
        inputVertical = 0.0f;
        inputHorizontal = 0.0f;
        inputPanning = 0.0f;
        inputZoom = 0.0f;
        SetName("FirstPersonCamera");
    }

    void FirstPersonCamera::OnUpdate()
    {
        if(!isControllable)
            return;

        inputVertical = Input::GetAxis("Vertical");
        inputHorizontal = Input::GetAxis("Horizontal");
        inputPanning = Input::GetAxis("Panning");
        inputZoom = Input::GetScrollDirection().y;
    }

    void FirstPersonCamera::OnLateUpdate()
    {
        if(!isControllable)
            return;

        Transform *transform = GetTransform();

        if(!isInitialized)
        {
            //Prevents camera from suddenly rotating
            auto euler = Quaternionf::ToEulerAngles(transform->GetRotation());
            currentRotation.x = euler.y;
            currentRotation.y = euler.x;
            isInitialized = true;
        }
        
        Vector3 direction = transform->GetForward() * inputVertical +
                            transform->GetRight() * inputHorizontal +
                            Vector3f::UnitY() * inputPanning;
        

        if(Vector3f::Length(direction) > 0)
        {
            direction = Vector3f::Normalize(direction);
            Move(direction, speed * Time::GetDeltaTime());
        }


        if (glm::abs(inputZoom) > std::numeric_limits<float>().min())
            Move(transform->GetForward(), inputZoom * zoomSpeed * Time::GetDeltaTime());

        Rotate();
    }

    void FirstPersonCamera::Move(const Vector3 &direction, float movementSpeed)
    {
        Transform *transform = GetTransform();
        Vector3 pos = transform->GetPosition();
        pos += direction * movementSpeed;
        transform->SetPosition(pos);
    }

    void FirstPersonCamera::Rotate()
    {
        if(!Input::GetButton(ButtonCode::Right))
            return;

        Transform *transform = GetTransform();

        Vector2 mouseDelta = Input::GetMouseDelta();

        currentRotation.y += -mouseDelta.x * rotationSpeed;
        currentRotation.x += -mouseDelta.y * rotationSpeed;

        currentRotation.x = glm::clamp(currentRotation.x, glm::radians(-89.9f), glm::radians(89.9f));

        Quaternion rotationY = Quaternionf::FromAxisAngle(Vector3(0.0f, 1.0f, 0.0f), currentRotation.y);
        Quaternion rotationX = Quaternionf::FromAxisAngle(Vector3(1.0f, 0.0f, 0.0f), currentRotation.x);
        Quaternion rotation = rotationY * rotationX;
        
        transform->SetRotation(rotation);
    }

    void FirstPersonCamera::ResetRotation()
    {
        Transform *transform = GetTransform();
        auto euler = Quaternionf::ToEulerAngles(transform->GetRotation());
        currentRotation.x = euler.y;
        currentRotation.y = euler.x;
    }

    void FirstPersonCamera::SetZoomSpeed(float zoomSpeed)
    {
        this->zoomSpeed = zoomSpeed;
    }

    float FirstPersonCamera::GetZoomSpeed() const
    {
        return zoomSpeed;
    }

    void FirstPersonCamera::SetSpeed(float speed)
    {
        this->speed = speed;
    }

    float FirstPersonCamera::GetSpeed() const
    {
        return speed;
    }

    void FirstPersonCamera::SetRotationSpeed(float rotationSpeed)
    {
        this->rotationSpeed = rotationSpeed;
    }

    float FirstPersonCamera::GetRotationSpeed() const
    {
        return rotationSpeed;
    }

    void FirstPersonCamera::SetIsControllable(bool isControllable)
    {
        this->isControllable = isControllable;
    }

    bool FirstPersonCamera::GetIsControllable() const
    {
        return isControllable;
    }
}