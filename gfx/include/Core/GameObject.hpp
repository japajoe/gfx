#ifndef GFX_GAMEOBJECT_HPP
#define GFX_GAMEOBJECT_HPP

#include "Object.hpp"
#include "Component.hpp"
#include "Transform.hpp"
#include <vector>
#include <memory>

namespace GFX
{
    enum Layer_
    {
        Layer_None = 0,
        Layer_Default = 1 << 0,
        Layer_IgnoreRaycast = 1 << 1,
        Layer_IgnoreCulling = 1 << 2
    };

	typedef int Layer;

    enum class PrimitiveType
    {
        Capsule,
        Cube,
        Cylinder,
        Hemisphere,
        Plane,
        Quad,
        ParticleSystem,
        ProceduralSkybox1,
        ProceduralSkybox2,
        Skybox,
        Sphere,
        Terrain,
        Water
    };

    class GameObject : public Object
    {
    friend class GameBehaviour;
    private:
        Transform transform;
        bool isActive;
        Layer layer;
        std::vector<std::unique_ptr<Component>> components;
        static std::vector<std::unique_ptr<GameObject>> objects;
        static std::vector<GameObject*> destroyQueue;
        static void DestroyImmediate(GameObject *object);
        static void OnEndFrame();
        static void RemoveObject(GameObject *object);
        static void DestroyAll();
    public:
        GameObject();
        ~GameObject();
        Transform *GetTransform();
        void SetIsActive(bool isActive);
        bool GetIsActive() const;
        void SetLayer(Layer layer, bool recursive = true);
        Layer GetLayer() const;
        static void Destroy(GameObject *object);        
        static GameObject *Create();
        static GameObject *CreatePrimitive(PrimitiveType type);

        template <typename T>
        T *GetComponent() const
        {
            static_assert(std::is_base_of<Component, T>::value, "GetComponent parameter must derive from Component");

            T *ptr = nullptr;

            for(size_t i = 0; i < components.size(); i++)
            {
                ptr = dynamic_cast<T*>(components[i].get());
                if(ptr)
                    return ptr;
            }
            return nullptr;
        }

        template <typename T>
        std::vector<T*> GetComponentsOfType() const
        {
            static_assert(std::is_base_of<Component, T>::value, "GetComponentsOfType parameter must derive from Component");

            std::vector<T*> targets;

            for(size_t i = 0; i < components.size(); i++)
            {
                T* ptr = dynamic_cast<T*>(components[i].get());
                if(ptr)
                {
                    targets.push_back(ptr);
                }
            }
            return targets;
        }

        template <typename T>
        std::vector<T*> GetComponentsOfTypeInChildren() const
        {
            static_assert(std::is_base_of<Component, T>::value, "GetComponentsOfTypeInChildren parameter must derive from Component");

            std::vector<T*> allComponents;

            // Get components of the current GameObject
            for (size_t i = 0; i < components.size(); i++)
            {
                T* ptr = dynamic_cast<T*>(components[i].get());
                if (ptr)
                {
                    allComponents.push_back(ptr);
                }
            }

            // Recursively get components from children using the Transform's GetChildren
            //auto &children = transform.GetChildren();

            Transform *child = nullptr;
            size_t index = 0;

            while((child = transform.GetChild(index++)) != nullptr)
            {
                auto childComponents = child->GetGameObject()->GetComponentsOfTypeInChildren<T>();
                allComponents.insert(allComponents.end(), childComponents.begin(), childComponents.end());
            }

            return allComponents;
        }

        // template <typename T, typename... Param>
        // T* AddComponent(Param... param)
        // {
        //     static_assert(std::is_base_of<Component, T>::value, "AddComponent parameter must derive from Component");

        //     std::unique_ptr<Component> ptr = std::make_unique<T>(param...);
        //     if (!ptr)
        //         return nullptr;

        //     components.push_back(std::move(ptr));
        //     size_t last = components.size() - 1;
        //     Component *component = components[last].get();
        //     component->gameObject = this;
        //     component->transform = &this->transform;
        //     component->OnInitialize();

        //     return static_cast<T*>(component);
        // }

        template<typename T, typename... Param>
        T* AddComponent(Param&&... param) 
        {
            static_assert(std::is_base_of<Component, T>::value, "AddComponent parameter must derive from Component");

            std::unique_ptr<Component> ptr = std::make_unique<T>(std::forward<Param>(param)...);
            if (!ptr)
                return nullptr;

            components.push_back(std::move(ptr));
            size_t last = components.size() - 1;
            Component* component = components[last].get();
            component->gameObject = this;
            component->transform = &this->transform;
            component->OnInitialize();

            return static_cast<T*>(component);
        }
    };
}

#endif