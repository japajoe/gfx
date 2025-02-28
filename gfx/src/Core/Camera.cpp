#include "Camera.hpp"
#include "Transform.hpp"
#include "GameObject.hpp"
#include "Resources.hpp"
#include "Constants.hpp"
#include "Input.hpp"
#include "../Graphics/Buffers/UniformBufferObject.hpp"
#include "../Graphics/Graphics.hpp"
#include "../External/glm/glm.hpp"

namespace GFX
{
    static bool setCallback = true;
    Camera *Camera::pMainCamera = nullptr;
	EventHandler<WindowResizeEvent> Graphics::windowResize;
    UniformBufferObject *Camera::ubo = nullptr;

    Camera::Camera() : Component()
    {
        this->fieldOfView = 70.0f;
        this->nearClippingPlane = 0.1f;
        this->farClippingPlane = 1000.0f;
        this->clearColor = Color(1.0f, 1.0f, 1.0f, 1.0f);
    }

    void Camera::OnInitialize()
    {
        if(pMainCamera == nullptr)
            pMainCamera = this;

        if(setCallback)
        {
            Graphics::windowResize += OnWindowResize;
            setCallback = false;
        }

        Initialize();
    }

    void Camera::OnDestroy()
    {
        if(pMainCamera == this)
        {
            pMainCamera = nullptr;
        }
    }

    void Camera::Initialize()
    {
        auto viewport = Graphics::GetViewport();
        float aspectRatio = viewport.width / viewport.height;
		projection = Matrix4f::Perspective(fieldOfView, aspectRatio, nearClippingPlane, farClippingPlane);
    }

    Matrix4 Camera::GetProjectionMatrix() const
    {
        return projection;
    }

    Matrix4 Camera::GetViewMatrix() const
    {
        return Matrix4f::Invert(GetTransform()->GetModelMatrix());
    }

    void Camera::SetFieldOfView(float fieldOfView)
    {
        this->fieldOfView = fieldOfView;
        Initialize();
    }

    float Camera::GetFieldOfView() const
    {
        return fieldOfView;
    }

    void Camera::SetNearClippingPlane(float nearClippingPlane)
    {
        this->nearClippingPlane = nearClippingPlane;
        Initialize();
    }

    float Camera::GetNearClippingPlane() const
    {
        return nearClippingPlane;
    }

    void Camera::SetFarClippingPlane(float farClippingPlane)
    {
        this->farClippingPlane = farClippingPlane;
        Initialize();
    }

    float Camera::GetFarClippingPlane() const
    {
        return farClippingPlane;
    }

    void Camera::SetClearColor(const Color &clearColor)
    {
        this->clearColor = clearColor;
    }

    Color Camera::GetClearColor() const
    {
        return clearColor;
    }

    Frustum *Camera::GetFrustum()
    {
        return &frustum;
    }

    Vector3 Camera::WorldToScreenPoint(const Vector3 &worldPoint)
    {
        Vector4 v(worldPoint.x, worldPoint.y, worldPoint.z, 1);
        Vector4 pointInNdc = GetProjectionMatrix() * GetViewMatrix() * v;
        pointInNdc /= pointInNdc.w;
        auto viewportRect = Graphics::GetViewport();
        float screenX = (pointInNdc.x + 1.0f) / 2.0f * viewportRect.width;
        float screenY = (1 - pointInNdc.y) / 2.0f * viewportRect.height;
        return Vector3(screenX, screenY, glm::abs(Vector3f::Distance(worldPoint, GetGameObject()->GetTransform()->GetPosition())));
    }

    bool Camera::WorldToScreenPoint(const Vector3 &worldPoint, Vector3 &screenPoint)
    {
        Vector4 v(worldPoint.x, worldPoint.y, worldPoint.z, 1);
        Vector4 pointInNdc = GetProjectionMatrix() * GetViewMatrix() * v;

        // Check if the point is behind the camera (negative w means behind)
        if (pointInNdc.w <= 0.0f)
        {
            return false;
        }

        pointInNdc /= pointInNdc.w; // Perspective divide (convert to NDC)

        // Check if the point is within the normalized device coordinates (-1 to 1)
        if (pointInNdc.x < -1.0f || pointInNdc.x > 1.0f ||
            pointInNdc.y < -1.0f || pointInNdc.y > 1.0f ||
            pointInNdc.z < 0.0f || pointInNdc.z > 1.0f) // z should be between 0 and 1 in NDC
        {
            return false;
        }

        auto viewportRect = Graphics::GetViewport();
        float screenX = (pointInNdc.x + 1.0f) / 2.0f * viewportRect.width;
        float screenY = (1 - pointInNdc.y) / 2.0f * viewportRect.height;
        float screenZ = glm::abs(Vector3f::Distance(worldPoint, GetGameObject()->GetTransform()->GetPosition()));

        screenPoint = Vector3(screenX, screenY, screenZ);
        return true;
    }

    Vector3 Camera::ScreenToWorldPoint(const Vector2 &screenPoint)
    {
        auto viewportRect = Graphics::GetViewport();
        float ndcX = (2.0f * screenPoint.x) / viewportRect.width - 1.0f;
        float ndcY = 1.0f - (2.0f * screenPoint.y) / viewportRect.height;
        float z = GetNearClippingPlane();
        Vector4 pointInNdc(ndcX, ndcY, z, 1);
        Matrix4 inverseProjection = Matrix4f::Invert(GetProjectionMatrix());
        Matrix4 inverseView = Matrix4f::Invert(GetViewMatrix());
        Vector4 pointInWorld = inverseView * inverseProjection * pointInNdc;
        pointInWorld /= pointInWorld.w;
        return Vector3(pointInWorld.x, pointInWorld.y, pointInWorld.z);
    }

    Camera *Camera::GetMain()
    {
        return pMainCamera;
    }

    void Camera::UpdateUniformBuffer()
    {
        if(ubo == nullptr)
            ubo = Resources::FindUniformBuffer(Constants::GetString(ConstantString::UniformBufferCamera));

        if(ubo == nullptr)
            return;

        Camera *camera = Camera::GetMain();

        if(camera == nullptr)
            return;

        camera->frustum.Initialize(camera->GetProjectionMatrix() * camera->GetViewMatrix());

        auto viewport = Graphics::GetViewport();

        UniformCameraInfo info;
        info.view = camera->GetViewMatrix();
        info.projection = camera->GetProjectionMatrix();
        info.viewProjection = info.projection * info.view;
        info.position = Vector4(camera->GetTransform()->GetPosition(), 1.0f);
        info.direction = Vector4(camera->GetTransform()->GetForward(), 1.0f);
        info.resolution = Vector4(viewport.width, viewport.height, 0, 0);
        info.mouse = Vector4(Input::GetMousePosition(), 0, 0);

        ubo->Bind();
        ubo->BufferSubData(0, sizeof(UniformCameraInfo), &info);
        ubo->Unbind();
    }

    void Camera::OnWindowResize(int width, int height)
    {
        if(pMainCamera)
            pMainCamera->Initialize();
    }
}