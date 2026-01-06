#include "Transform.hpp"
#include "GameObject.hpp"
#include "Time.hpp"
#include <algorithm>

namespace GFX
{
    // Transform::Transform() : Component()
    // {
    //     parent = nullptr;
    //     root = this;        
        
    //     localPosition = Vector3(0, 0, 0);
    //     localScale = Vector3(1, 1, 1);
    //     localRotation = Quaternion(1, 0, 0, 0);
        
    //     oldPosition = Vector3(0, 0, 0);

    //     newRotation = Vector3(0, 0, 0);

    //     velocity = Vector3(0, 0, 0);

    //     SetTranslationMatrix();
    //     SetRotationMatrix();
    //     SetScaleMatrix();

    //     SetName("Transform");
    // }

    // std::vector<Transform*> &Transform::GetChildren()
    // {
    //     return children;
    // }

    // std::vector<Transform*> Transform::GetChildrenRecursive() const
    // {
    //     std::vector<Transform*> allChildren;

    //     for (const auto& child : children)
    //     {
    //         allChildren.push_back(child);
    //         std::vector<Transform*> childChildren = child->GetChildrenRecursive();
    //         allChildren.insert(allChildren.end(), childChildren.begin(), childChildren.end());
    //     }

    //     return allChildren;
    // }

    // Transform *Transform::GetChild(size_t index) const
    // {
    //     if(index >= children.size())
    //         return nullptr;
    //     return children[index];
    // }

    // void Transform::SetTranslationMatrix()
    // {
    //     translationMatrix = Matrix4f::CreateTranslation(localPosition.x, localPosition.y, localPosition.z);
    //     SetModelMatrix();
    // }

    // void Transform::SetRotationMatrix()
    // {
    //     rotationMatrix = Matrix4f::CreateRotation(localRotation.x, localRotation.y, localRotation.z, localRotation.w);
    //     SetModelMatrix();
    // }

    // void Transform::SetScaleMatrix()
    // {
    //     scaleMatrix = Matrix4f::CreateScale(localScale.x, localScale.y, localScale.z);
    //     SetModelMatrix();
    // }

    // void Transform::SetModelMatrix()
    // {
    //     modelMatrix = translationMatrix * rotationMatrix * scaleMatrix;
    // }

    // Matrix4 Transform::GetModelMatrix() const
    // {
    //     if (parent != nullptr)
    //     {
    //         return parent->GetModelMatrix() * modelMatrix;
    //     }
    //     else
    //     {
    //         return modelMatrix;
    //     }
    // }

    // void Transform::SetPosition(const Vector3 &value)
    // {
    //     if (parent != nullptr)
    //     {
    //         auto localToWorld = Matrix4f::Invert(GetModelMatrix());
    //         auto localPosition = GetLocalPosition() + Vector3f::TransformPosition(value, localToWorld);
    //         SetLocalPosition(localPosition);
    //     }
    //     else
    //     {
    //         SetLocalPosition(value);
    //     }
    // }

    // Vector3 Transform::GetPosition() const
    // {
    //     if(parent != nullptr)
    //     {
    //         auto localMatrix = GetModelMatrix();
    //         return Matrix4f::ExtractTranslation(localMatrix);
    //     }
    //     else
    //     {
    //         return GetLocalPosition();
    //     }
    // }

    // void Transform::SetLocalPosition(const Vector3 &value)
    // {
    //     oldPosition = localPosition;    
    //     localPosition = value;

    //     float deltaTime = Time::GetDeltaTime();

    //     float dx = localPosition.x - oldPosition.x;
    //     float dy = localPosition.y - oldPosition.y;
    //     float dz = localPosition.z - oldPosition.z;
    //     velocity = Vector3(dx / deltaTime, dy / deltaTime, dz / deltaTime);

    //     SetTranslationMatrix();
    // }

    // Vector3 Transform::GetLocalPosition() const
    // {
    //     return localPosition;
    // }

    // void Transform::SetRotation(const Quaternion &value)
    // {
    //     if(parent != nullptr)
    //     {
    //         SetLocalRotation(Quaternionf::Invert(parent->GetRotation()) * value);
    //     }
    //     else
    //     {
    //         SetLocalRotation(value);
    //     }
    // }

    // Quaternion Transform::GetRotation() const
    // {
    //     if(parent != nullptr)
    //     {
    //         auto localToWorld = GetModelMatrix();
    //         return Matrix4f::ExtractRotation(localToWorld);
    //     }
    //     else
    //     {
    //         return GetLocalRotation();
    //     }
    // }

    // void Transform::SetLocalRotation(const Quaternion &value)
    // {
    //     localRotation = value;
    //     SetRotationMatrix();
    // }

    // Quaternion Transform::GetLocalRotation() const
    // {
    //     return localRotation;
    // }

    // void Transform::SetScale(const Vector3 &value)
    // {
    //     SetLocalScale(value);
    // }

    // Vector3 Transform::GetScale() const
    // {
    //     if (parent != nullptr)
    //     {
    //         auto localToWorld = GetModelMatrix();
    //         return Matrix4f::ExtractScale(localToWorld);
    //     }
    //     else
    //     {
    //         return GetLocalScale();
    //     }
    // }

    // void Transform::SetLocalScale(const Vector3 &value)
    // {
    //     localScale = value;
    //     SetScaleMatrix();
    // }

    // Vector3 Transform::GetLocalScale() const
    // {
    //     return localScale;
    // }

    // Vector3 Transform::GetVelocity()
    // {
    //     return velocity;
    // }

    // Vector3 Transform::GetForward() const
    // {
    //     auto result = GetRotation() * -Vector3f::UnitZ();
    //     if(Vector3f::IsNan(result))
    //     {
    //         return Vector3f::Zero();
    //     }
    //     return result;
    // }

    // Vector3 Transform::GetUp() const
    // {
    //     auto result = GetRotation() * Vector3f::UnitY();
    //     if(Vector3f::IsNan(result))
    //     {
    //         return Vector3f::Zero();
    //     }
    //     return result;
    // }

    // Vector3 Transform::GetRight() const
    // {
    //     auto result = GetRotation() * Vector3f::UnitX();
    //     if(Vector3f::IsNan(result))
    //     {
    //         return Vector3f::Zero();
    //     }
    //     return result;
    // }

    // Transform *Transform::GetParent() const
    // {
    //     return parent;
    // }

    // void Transform::SetParent(Transform *newParent)
    // {
    //     if(newParent == this)
    //         return; //Can't set parent to self

    //     if (parent != nullptr)
    //     {
    //         auto it = std::find(parent->children.begin(), parent->children.end(), this);
            
    //         if (it != parent->children.end())
    //         {
    //             parent->children.erase(it);
    //         }
    //     }

    //     parent = newParent;

    //     if (parent != nullptr)
    //         parent->children.push_back(this);

    //     Transform *currentRoot = this;
        
    //     while (currentRoot->parent != nullptr)
    //     {
    //         currentRoot = currentRoot->parent;
    //     }

    //     root = currentRoot;
    // }

    // Transform *Transform::GetRoot() const
    // {
    //     return root;
    // }

    // Vector3 Transform::TransformDirection(const Vector3 &direction)
    // {
    //     return GetRotation() * direction;
    // }

    // Vector3 Transform::InverseTransformDirection(const Vector3 &direction)
    // {
    //     Vector3 dir = Matrix3(Matrix4f::Invert(GetModelMatrix())) * direction;
    //     return Vector3f::Normalize(dir);
    // }

    // Vector3 Transform::InverseTransformPoint(const Vector3 &point)
    // {
    //     // Step 1: Translate the point to the origin
    //     Vector3 pointRelativeToOrigin = point - GetPosition();

    //     // Step 2: Rotate the point back using the inverse of the rotation
    //     Quaternion inverseRotation = Quaternionf::Invert(GetRotation());
    //     Vector3 localPoint = Vector3f::Transform(pointRelativeToOrigin, inverseRotation);

    //     // Step 3: Scale the point back (if needed)
    //     // Note: Scaling is usually applied in the forward transform, so we may not need to apply it here.
    //     // If you want to consider scaling, you can divide by the scale factors.
    //     Vector3 scale = GetScale();
    //     localPoint.x /= scale.x;
    //     localPoint.y /= scale.y;
    //     localPoint.z /= scale.z;

    //     return localPoint;
    // }

    // void Transform::LookAt(Transform *target)
    // {
    //     LookAt(target, Vector3f::UnitY());
    // }

    // void Transform::LookAt(Transform *target, const Vector3 &worldUp)
    // {
    //     if (!target)
    //         return;
    //     LookAt(target->GetPosition(), worldUp);
    // }

    // void Transform::LookAt(const Vector3 &worldPosition, const Vector3 &worldUp)
    // {
    //     auto pos = GetPosition();
    //     Matrix4 mat = Matrix4f::LookAt(pos, worldPosition, worldUp);
    //     Quaternion rot = Quaternionf::FromMatrix(Matrix3(mat));
    //     Rotate(rot);
    // }

    // void Transform::Rotate(const Quaternion &rotation)
    // {
    //     SetRotation(rotation);
    // }

    // void Transform::Rotate(const Vector3 &rotation)
    // {
    //     auto previousRotation = Quaternionf::ToEulerAngles(GetRotation());

    //     newRotation.x += rotation.x;
    //     newRotation.y += rotation.y;
    //     newRotation.z += rotation.z;

    //     auto rotationDelta = newRotation - previousRotation;

    //     auto rott = GetRotation();

    //     rott = rott * Quaternionf::FromAxisAngle(Vector3f::UnitX(), glm::radians(rotationDelta.x));
    //     rott = rott * Quaternionf::FromAxisAngle(Vector3f::UnitY(), glm::radians(rotationDelta.y));
    //     rott = rott * Quaternionf::FromAxisAngle(Vector3f::UnitZ(), glm::radians(-rotationDelta.z));

    //     newRotation = Quaternionf::ToEulerAngles(rott);
    //     SetRotation(rott);
    // }

    // Transform *Transform::FindChild(const std::string &name)
    // {
    //     for(const auto& child : children)
    //     {
    //         if(child->GetGameObject()->GetName() == name)
    //         {
    //             return child;
    //         }
    //         else
    //         {
    //             Transform* foundChild = child->FindChild(name);
    //             if (foundChild != nullptr)
    //             {
    //                 return foundChild;
    //             }
    //         }
    //     }

    //     return nullptr;
    // }

    // Vector3 Transform::WorldToLocal(const Vector3 &v)
    // {
    //     auto invScale = GetScale();
    //     if (invScale.x != 0.0f)
    //         invScale.x = 1.0f / invScale.x;
    //     if (invScale.y != 0.0f)
    //         invScale.y = 1.0f / invScale.y;
    //     if (invScale.z != 0.0f)
    //         invScale.z = 1.0f / invScale.z;
    //     const Quaternion invRotation = Quaternionf::Conjugate(GetRotation());
    //     Vector3 result = v - GetPosition();
    //     result = Vector3f::Transform(result, invRotation);
    //     result *= invScale;
    //     return result;
    // }

    // Vector3 Transform::WorldToLocalVector(const Vector3 &v)
    // {
    //     auto invScale = GetScale();
    //     if (invScale.x != 0.0f)
    //         invScale.x = 1.0f / invScale.x;
    //     if (invScale.y != 0.0f)
    //         invScale.y = 1.0f / invScale.y;
    //     if (invScale.z != 0.0f)
    //         invScale.z = 1.0f / invScale.z;
    //     const Quaternion invRotation = Quaternionf::Conjugate(GetRotation());
    //     Vector3 result = Vector3f::Transform(v, invRotation);
    //     result *= invScale;
    //     return result;
    // }

    // Vector3 Transform::LocalToWorld(const Vector3 &v)
    // {
    //     Vector3 tmp = v * GetScale();
    //     tmp = Vector3f::Transform(tmp, GetRotation());
    //     return tmp + GetPosition();
    // }

    // Vector3 Transform::LocalToWorldVector(const Vector3 &v)
    // {
    //     Vector3 tmp = v * GetScale();
    //     return Vector3f::Transform(tmp, GetRotation());
    // }





























    Transform::Transform() : Component()
    {
        localPosition = Vector3(0, 0, 0);
        localRotation = glm::quat_identity<float, glm::defaultp>();
        localScale = Vector3(1, 1, 1);
        parent = nullptr;
        root = this;
        isDirty = true;

        SetName("Transform");
    }

    std::vector<Transform*> &Transform::GetChildren()
    {
        return children;
    }

    std::vector<Transform*> Transform::GetChildrenRecursive() const
    {
        std::vector<Transform*> allChildren;

        for (const auto& child : children)
        {
            allChildren.push_back(child);
            std::vector<Transform*> childChildren = child->GetChildrenRecursive();
            allChildren.insert(allChildren.end(), childChildren.begin(), childChildren.end());
        }

        return allChildren;
    }

    Transform *Transform::GetChild(size_t index) const
    {
        if(index >= children.size())
            return nullptr;
        return children[index];
    }

    Matrix4 Transform::GetModelMatrix() const
    {
        if (isDirty)
        {
            RecalculateModelMatrix();
        }
        return cachedWorldMatrix;
    }

    void Transform::RecalculateModelMatrix() const
    {
        Matrix4 localMatrix = glm::translate(glm::mat4(1.0f), localPosition) * glm::toMat4(localRotation) * glm::scale(glm::mat4(1.0f), localScale);

        if (parent)
        {
            cachedWorldMatrix = parent->GetModelMatrix() * localMatrix;
        }
        else
        {
            cachedWorldMatrix = localMatrix;
        }

        isDirty = false;
    }

    void Transform::SetPosition(const Vector3 &value)
    {
        previousPosition = localPosition;

        if (parent)
        {
            Matrix4 parentMatrix = parent->GetModelMatrix();
            Matrix4 invParentMatrix = glm::inverse(parentMatrix);
            localPosition = glm::vec3(invParentMatrix * glm::vec4(value, 1.0f));
        }
        else
        {
            localPosition = value;
        }

        float deltaTime = Time::GetDeltaTime();

        float dx = localPosition.x - previousPosition.x;
        float dy = localPosition.y - previousPosition.y;
        float dz = localPosition.z - previousPosition.z;
        velocity = Vector3(dx / deltaTime, dy / deltaTime, dz / deltaTime);

        MarkDirty();
    }

    Vector3 Transform::GetPosition() const
    {
	    return glm::vec3(GetModelMatrix()[3]);
    }

    void Transform::SetLocalPosition(const Vector3 &value)
    {
        localPosition = value;
        MarkDirty();
    }

    Vector3 Transform::GetLocalPosition() const
    {
        return localPosition;
    }

    void Transform::SetRotation(const Quaternion &value)
    {
        if (parent)
        {
            localRotation = glm::inverse(parent->GetRotation()) * value;
        }
        else
        {
            localRotation = value;
        }
        MarkDirty();
    }

    Quaternion Transform::GetRotation() const
    {
        if (parent)
        {
            return parent->GetRotation() * localRotation;
        }
        else
        {
            return localRotation;
        }
    }

    void Transform::SetLocalRotation(const Quaternion &value)
    {
        localRotation = value;
        MarkDirty();
    }

    Quaternion Transform::GetLocalRotation() const
    {
        return localRotation;
    }

    void Transform::SetScale(const Vector3 &value)
    {
        localScale = value;
        MarkDirty();
    }

    Vector3 Transform::GetScale() const
    {
        return localScale;
    }

    Vector3 Transform::GetLocalScale() const
    {
        return localScale;
    }

    Vector3 Transform::GetVelocity()
    {
        return velocity;
    }

    Vector3 Transform::GetForward() const
    {
        auto result = GetRotation() * -Vector3f::UnitZ();
        if(Vector3f::IsNan(result))
        {
            return Vector3f::Zero();
        }
        return result;
    }

    Vector3 Transform::GetUp() const
    {
        auto result = GetRotation() * Vector3f::UnitY();
        if(Vector3f::IsNan(result))
        {
            return Vector3f::Zero();
        }
        return result;
    }

    Vector3 Transform::GetRight() const
    {
        auto result = GetRotation() * Vector3f::UnitX();
        if(Vector3f::IsNan(result))
        {
            return Vector3f::Zero();
        }
        return result;
    }

    Transform *Transform::GetParent() const
    {
        return parent;
    }

    void Transform::SetParent(Transform *newParent)
    {
        if(newParent == this)
            return; //Can't set parent to self

        if (parent != nullptr)
        {
            auto it = std::find(parent->children.begin(), parent->children.end(), this);
            
            if (it != parent->children.end())
            {
                parent->children.erase(it);
            }
        }

        parent = newParent;

        if (parent != nullptr)
            parent->children.push_back(this);

        Transform *currentRoot = this;
        
        while (currentRoot->parent != nullptr)
        {
            currentRoot = currentRoot->parent;
        }

        root = currentRoot;
    }

    Transform *Transform::GetRoot() const
    {
        return root;
    }

    Vector3 Transform::TransformDirection(const Vector3 &direction)
    {
        return GetRotation() * direction;
    }

    Vector3 Transform::InverseTransformDirection(const Vector3 &direction)
    {
        Vector3 dir = Matrix3(Matrix4f::Invert(GetModelMatrix())) * direction;
        return Vector3f::Normalize(dir);
    }

    Vector3 Transform::InverseTransformPoint(const Vector3 &point)
    {
        // Step 1: Translate the point to the origin
        Vector3 pointRelativeToOrigin = point - GetPosition();

        // Step 2: Rotate the point back using the inverse of the rotation
        Quaternion inverseRotation = Quaternionf::Invert(GetRotation());
        Vector3 localPoint = Vector3f::Transform(pointRelativeToOrigin, inverseRotation);

        // Step 3: Scale the point back (if needed)
        // Note: Scaling is usually applied in the forward transform, so we may not need to apply it here.
        // If you want to consider scaling, you can divide by the scale factors.
        Vector3 scale = GetScale();
        localPoint.x /= scale.x;
        localPoint.y /= scale.y;
        localPoint.z /= scale.z;

        return localPoint;
    }

    void Transform::LookAt(Transform *target)
    {
        LookAt(target, Vector3f::UnitY());
    }

    void Transform::LookAt(Transform *target, const Vector3 &worldUp)
    {
        if (!target)
            return;
        LookAt(target->GetPosition(), worldUp);
    }

    void Transform::LookAt(const Vector3 &worldPosition, const Vector3 &worldUp)
    {
        auto pos = GetPosition();
        Matrix4 mat = Matrix4f::LookAt(pos, worldPosition, worldUp);
        Quaternion rot = Quaternionf::FromMatrix(Matrix3(mat));
        Rotate(rot);
    }

    void Transform::Rotate(const Quaternion &rotation)
    {
        SetRotation(rotation);
    }

    void Transform::Rotate(const Vector3 &rotation)
    {
        auto previousRotation = Quaternionf::ToEulerAngles(GetRotation());

        newRotation.x += rotation.x;
        newRotation.y += rotation.y;
        newRotation.z += rotation.z;

        auto rotationDelta = newRotation - previousRotation;

        auto rott = GetRotation();

        rott = rott * Quaternionf::FromAxisAngle(Vector3f::UnitX(), glm::radians(rotationDelta.x));
        rott = rott * Quaternionf::FromAxisAngle(Vector3f::UnitY(), glm::radians(rotationDelta.y));
        rott = rott * Quaternionf::FromAxisAngle(Vector3f::UnitZ(), glm::radians(-rotationDelta.z));

        newRotation = Quaternionf::ToEulerAngles(rott);
        SetRotation(rott);
    }

    Transform *Transform::FindChild(const std::string &name)
    {
        for(const auto& child : children)
        {
            if(child->GetGameObject()->GetName() == name)
            {
                return child;
            }
            else
            {
                Transform* foundChild = child->FindChild(name);
                if (foundChild != nullptr)
                {
                    return foundChild;
                }
            }
        }

        return nullptr;
    }

    Vector3 Transform::WorldToLocal(const Vector3 &v)
    {
        auto invScale = GetScale();
        if (invScale.x != 0.0f)
            invScale.x = 1.0f / invScale.x;
        if (invScale.y != 0.0f)
            invScale.y = 1.0f / invScale.y;
        if (invScale.z != 0.0f)
            invScale.z = 1.0f / invScale.z;
        const Quaternion invRotation = Quaternionf::Conjugate(GetRotation());
        Vector3 result = v - GetPosition();
        result = Vector3f::Transform(result, invRotation);
        result *= invScale;
        return result;
    }

    Vector3 Transform::WorldToLocalVector(const Vector3 &v)
    {
        auto invScale = GetScale();
        if (invScale.x != 0.0f)
            invScale.x = 1.0f / invScale.x;
        if (invScale.y != 0.0f)
            invScale.y = 1.0f / invScale.y;
        if (invScale.z != 0.0f)
            invScale.z = 1.0f / invScale.z;
        const Quaternion invRotation = Quaternionf::Conjugate(GetRotation());
        Vector3 result = Vector3f::Transform(v, invRotation);
        result *= invScale;
        return result;
    }

    Vector3 Transform::LocalToWorld(const Vector3 &v)
    {
        Vector3 tmp = v * GetScale();
        tmp = Vector3f::Transform(tmp, GetRotation());
        return tmp + GetPosition();
    }

    Vector3 Transform::LocalToWorldVector(const Vector3 &v)
    {
        Vector3 tmp = v * GetScale();
        return Vector3f::Transform(tmp, GetRotation());
    }

    void Transform::MarkDirty()
    {
        if (!isDirty)
        {
            isDirty = true;
            for (Transform *child : children)
            {
                child->MarkDirty();
            }
        }
    }

}