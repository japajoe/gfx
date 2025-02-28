#include "RaycastHit.hpp"
#include "../System/Numerics/Matrix4.hpp"
#include "../Core/Camera.hpp"
#include "../Core/Input.hpp"
#include "../Graphics/Graphics.hpp"

namespace GFX
{
    Ray::Ray()
    {
        origin = Vector3f::Zero();
        direction = -Vector3f::UnitZ();
        length = 10000;
    }

    Ray::Ray(const Vector3 &origin, const Vector3 &direction, float length)
    {
        this->origin = origin;
        this->direction = direction;
        this->length = length;
    }

    //Ray Ray::FromMousePosition(const Vector2 &mousePosition, const Rectangle &viewportRect)
    Ray Ray::FromMousePosition()
    {
        auto mousePosition = Input::GetMousePosition();
        auto viewportRect = Graphics::GetViewport();
        Vector2 viewportPosition(viewportRect.x, viewportRect.y);
        Vector2 relativeMousePosition = mousePosition - viewportPosition;

        float mouseX = relativeMousePosition.x;
        float mouseY = viewportRect.height - relativeMousePosition.y;

        Vector4 rayStartNDC(
            (mouseX / viewportRect.width - 0.5f) * 2.0f,  // [0,1024] -> [-1,1]
            (mouseY / viewportRect.height - 0.5f) * 2.0f, // [0, 768] -> [-1,1]
            -1.0, // The near plane maps to Z=-1 in Normalized Device Coordinates
            1.0f
        );

        Vector4 rayEndNDC(
            (mouseX /  viewportRect.width - 0.5f) * 2.0f,
            (mouseY /  viewportRect.height - 0.5f) * 2.0f,
            0.0,
            1.0f
        );

        Camera *mainCamera = Camera::GetMain();
        Matrix4 viewMatrix = mainCamera->GetViewMatrix();
        Matrix4 projectionMatrix = mainCamera->GetProjectionMatrix();

        Matrix4 m = Matrix4f::Invert(projectionMatrix * viewMatrix);
        Vector4 rayStartWorld = m * rayStartNDC; 
        rayStartWorld /= rayStartWorld.w;
        Vector4 rayEndWorld = m * rayEndNDC;
        rayEndWorld /= rayEndWorld.w;

        Vector3 rayDirWorld(rayEndWorld - rayStartWorld);
        rayDirWorld = Vector3f::Normalize(rayDirWorld);

        Vector3 origin = Vector3(rayStartWorld);
        Vector3 direction = Vector3f::Normalize(rayDirWorld);

        return Ray(origin, direction, mainCamera->GetFarClippingPlane());
    }
}