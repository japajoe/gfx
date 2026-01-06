#ifndef GFX_TRANSFORM_HPP
#define GFX_TRANSFORM_HPP

#include "Component.hpp"
#include "../System/Numerics/Vector3.hpp"
#include "../System/Numerics/Quaternion.hpp"
#include "../System/Numerics/Matrix3.hpp"
#include "../System/Numerics/Matrix4.hpp"
#include <vector>

namespace GFX
{
    struct TransformData
    {
        Vector3 position;
        Quaternion rotation;
        Vector3 scale;
    };

    // class Transform : public Component
    // {
    // private:
    //     Transform *parent;
    //     Transform *root;        
    //     std::vector<Transform*> children;        
    //     Matrix4 translationMatrix;
    //     Matrix4 rotationMatrix;
    //     Matrix4 scaleMatrix;
    //     Matrix4 modelMatrix;
    //     Vector3 localPosition;
    //     Vector3 localScale;
    //     Quaternion localRotation;
    //     Vector3 oldPosition;
    //     Vector3 newRotation;
    //     Vector3 velocity;
    //     void SetTranslationMatrix();
    //     void SetRotationMatrix();
    //     void SetScaleMatrix();
    //     void SetModelMatrix(); 
    // public:
    //     Transform();
    //     Transform(const Transform &other);
    //     Transform(Transform &&other) noexcept;
    //     Transform& operator=(const Transform &other);
    //     Transform& operator=(Transform &&other) noexcept;
    //     std::vector<Transform*> &GetChildren();
    //     std::vector<Transform*> GetChildrenRecursive() const;
    //     Transform *GetChild(size_t index) const;
    //     Matrix4 GetModelMatrix() const;
    //     void SetPosition(const Vector3 &value);
    //     Vector3 GetPosition() const;
    //     void SetLocalPosition(const Vector3 &value);
    //     Vector3 GetLocalPosition() const;
    //     void SetRotation(const Quaternion &value);
    //     Quaternion GetRotation() const;
    //     void SetLocalRotation(const Quaternion &value);
    //     Quaternion GetLocalRotation() const;
    //     void SetScale(const Vector3 &value);
    //     Vector3 GetScale() const;
    //     void SetLocalScale(const Vector3 &value);
    //     Vector3 GetLocalScale() const;
    //     Vector3 GetVelocity();
    //     Vector3 GetForward() const;
    //     Vector3 GetUp() const;
    //     Vector3 GetRight() const;
    //     Transform *GetParent() const;
    //     void SetParent(Transform *newParent);
    //     Transform *GetRoot() const;
    //     Vector3 TransformDirection(const Vector3 &direction);
    //     Vector3 InverseTransformDirection(const Vector3 &direction);
    //     Vector3 InverseTransformPoint(const Vector3 &point);
    //     void LookAt(Transform *target);
    //     void LookAt(Transform *target, const Vector3 &worldUp);
    //     void LookAt(const Vector3 &worldPosition, const Vector3 &worldUp);
    //     void Rotate(const Quaternion &rotation);
    //     void Rotate(const Vector3 &rotation);
    //     Transform *FindChild(const std::string &name);
    //     Vector3 WorldToLocal(const Vector3 &v);
    //     Vector3 WorldToLocalVector(const Vector3 &v);
    //     Vector3 LocalToWorld(const Vector3 &v);
    //     Vector3 LocalToWorldVector(const Vector3 &v);  
    // };


    class Transform : public Component
    {
        private:
        Vector3 localPosition;
        Quaternion localRotation;
        Vector3 localScale;
        Vector3 velocity;
        Vector3 previousPosition;
        Vector3 newRotation;
        Transform* parent;
        Transform* root;
        std::vector<Transform*> children;
        mutable Matrix4 cachedWorldMatrix;
        mutable bool isDirty;
        void RecalculateModelMatrix() const;
        void MarkDirty();
    public:
        Transform();
        std::vector<Transform*> &GetChildren();
        std::vector<Transform*> GetChildrenRecursive() const;
        Transform *GetChild(size_t index) const;
        Matrix4 GetModelMatrix() const;
        void SetPosition(const Vector3 &value);
        Vector3 GetPosition() const;
        void SetLocalPosition(const Vector3 &value);
        Vector3 GetLocalPosition() const;
        void SetRotation(const Quaternion &value);
        Quaternion GetRotation() const;
        void SetLocalRotation(const Quaternion &value);
        Quaternion GetLocalRotation() const;
        void SetScale(const Vector3 &value);
        Vector3 GetScale() const;
        Vector3 GetLocalScale() const;
        Vector3 GetVelocity();
        Vector3 GetForward() const;
        Vector3 GetUp() const;
        Vector3 GetRight() const;
        Transform *GetParent() const;
        void SetParent(Transform *newParent);
        Transform *GetRoot() const;
        Vector3 TransformDirection(const Vector3 &direction);
        Vector3 InverseTransformDirection(const Vector3 &direction);
        Vector3 InverseTransformPoint(const Vector3 &point);
        void LookAt(Transform *target);
        void LookAt(Transform *target, const Vector3 &worldUp);
        void LookAt(const Vector3 &worldPosition, const Vector3 &worldUp);
        void Rotate(const Quaternion &rotation);
        void Rotate(const Vector3 &rotation);
        Transform *FindChild(const std::string &name);
        Vector3 WorldToLocal(const Vector3 &v);
        Vector3 WorldToLocalVector(const Vector3 &v);
        Vector3 LocalToWorld(const Vector3 &v);
        Vector3 LocalToWorldVector(const Vector3 &v);  
    };
}

#endif