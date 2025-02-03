#ifndef GFX_FIRSTPERSONCAMERA_HPP
#define GFX_FIRSTPERSONCAMERA_HPP

#include "../System/Numerics/Vector3.hpp"
#include "GameBehaviour.hpp"

namespace GFX
{
    class FirstPersonCamera : public GameBehaviour
    {
    public:
        void ResetRotation();
        void SetZoomSpeed(float zoomSpeed);
        float GetZoomSpeed() const;
        void SetSpeed(float speed);
        float GetSpeed() const;
        void SetRotationSpeed(float rotationSpeed);
        float GetRotationSpeed() const;
        void SetIsControllable(bool isControllable);
        bool GetIsControllable() const;
    protected:
        void OnInitialize() override;
        void OnUpdate() override;
        void OnLateUpdate() override;
    private:
        bool isInitialized;
        float zoomSpeed;
        float speed;
        float rotationSpeed;
        bool isControllable;
        float inputVertical;
        float inputHorizontal;
        float inputPanning;
        float inputZoom;
        Vector3 currentRotation;
        void Move(const Vector3 &direction, float movementSpeed);
        void Rotate();
    };
}

#endif