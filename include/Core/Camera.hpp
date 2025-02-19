#ifndef GFX_CAMERA_HPP
#define GFX_CAMERA_HPP

#include "Component.hpp"
#include "../System/Numerics/Matrix4.hpp"
#include "../System/Numerics/Vector2.hpp"
#include "../System/Numerics/Vector3.hpp"
#include "../System/Numerics/Vector4.hpp"
#include "../Graphics/Color.hpp"
#include "../Graphics/Frustum.hpp"

namespace GFX
{
    class UniformBufferObject;

    struct UniformCameraInfo
    {
        Matrix4 view;
        Matrix4 projection;
        Matrix4 viewProjection;
        Vector4 position;
        Vector4 direction;
        Vector4 resolution;
        Vector4 mouse;
    };

    class Camera : public Component
    {
    friend class Graphics;
    private:
        static Camera *pMainCamera;
        Frustum frustum;
        Matrix4 projection;
        Color clearColor;
        float fieldOfView;
        float nearClippingPlane;
        float farClippingPlane;
        void Initialize();
        static UniformBufferObject *ubo;
        static void OnWindowResize(int width, int height);
        static void UpdateUniformBuffer();
    protected:
        void OnInitialize() override;
        void OnDestroy() override;
    public:
        Camera();
        Matrix4 GetProjectionMatrix() const;
        Matrix4 GetViewMatrix() const;
        void SetFieldOfView(float fieldOfView);
        float GetFieldOfView() const;
        void SetNearClippingPlane(float nearClippingPlane);
        float GetNearClippingPlane() const;
        void SetFarClippingPlane(float farClippingPlane);
        float GetFarClippingPlane() const;
        void SetClearColor(const Color &clearColor);
        Color GetClearColor() const;
        Frustum *GetFrustum();
        Vector3 WorldToScreenPoint(const Vector3 &worldPoint);
        bool WorldToScreenPoint(const Vector3 &worldPoint, Vector3 &screenPoint);
        Vector3 ScreenToWorldPoint(const Vector2 &screenPoint);
        static Camera *GetMain();        
    };
}

#endif