#ifndef GFX_MOUSEORBIT_HPP
#define GFX_MOUSEORBIT_HPP

#include "GameBehaviour.hpp"
#include "../System/Numerics/Vector2.hpp"
#include "../System/Numerics/Vector3.hpp"
#include "../System/Numerics/Quaternion.hpp"

namespace GFX
{
    class Camera;
    class Transform;

    struct MinMaxConstraint
    {
        float min;
        float max;

        MinMaxConstraint()
        {
            min = 0;
            max = 0;
        }

        MinMaxConstraint(float min, float max)
        {
            this->min = min;
            this->max = max;
        }
    };

    class MouseOrbit : public GameBehaviour
    {
    private:
        Camera *camera;
        float xVelocity;
        float yVelocity;
        float xRotationAxis;
        float yRotationAxis;
        float zoomVelocityZAxis;
        bool canControl;
        Vector2 mousePositionPrevious;
        Vector2 mousePositionCurrent;
        void OnToggledInput(bool enabled);
        float ClampAngleBetweenMinAndMax(float angle, float min, float max);
        Quaternion Euler(float x, float y, float z);
    protected:
        void OnInitialize() override;
        void OnUpdate() override;
        void OnLateUpdate() override;
    public:
        bool autoRotate;
        float rotationSpeed;
        float startRotation;
        float rotationSmoothing;
        Transform *target;
        float rotationSensitivity;
        MinMaxConstraint rotationLimit;
        float zAxisDistance;
        MinMaxConstraint cameraZoomRangeZAxis;
        float zoomSmoothness;
        float zoomSensitivity;
        bool canRotate;
        void ToggleCanControl(bool enabled);
        void SetTarget(Transform *target);
    };
}

#endif