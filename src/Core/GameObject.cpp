#include "GameObject.hpp"
#include "Resources.hpp"
#include "Constants.hpp"
#include "../Graphics/Mesh.hpp"
#include "../Graphics/Texture2D.hpp"
#include "../Graphics/Materials/DiffuseMaterial.hpp"
#include "../Graphics/Renderers/MeshRenderer.hpp"

namespace GFX
{
    std::vector<std::unique_ptr<GameObject>> GameObject::objects;
    std::vector<GameObject*> GameObject::destroyQueue;

    GameObject::GameObject() : Object()
    {
        transform.gameObject = this;
        isActive = true;
        layer = Layer_Default;
    }

    GameObject::~GameObject()
    {
        for(size_t i = 0; i < components.size(); i++)
        {
            components[i]->OnDestroy();
        }
        components.clear();
    }

    Transform *GameObject::GetTransform()
    {
        return &transform;
    }

    void GameObject::SetIsActive(bool isActive)
    {
        this->isActive = isActive;

        auto &children = transform.GetChildren();

        for(size_t i = 0; i < children.size(); i++)
        {
            children[i]->GetGameObject()->SetIsActive(isActive);
        }

        if(isActive)
        {
            for(size_t i = 0; i < components.size(); i++)
            {
                components[i]->OnActivate();
            }
        }
        else
        {
            for(size_t i = 0; i < components.size(); i++)
            {
                components[i]->OnDeactivate();
            }
        }
    }

    bool GameObject::GetIsActive() const
    {
        return isActive;
    }

    void GameObject::SetLayer(Layer layer, bool recursive)
    {
        this->layer = layer;

        if(recursive)
        {
            auto &children = transform.GetChildren();

            for(size_t i = 0; i < children.size(); i++)
            {
                children[i]->GetGameObject()->SetLayer(layer);
            }
        }
    }

    Layer GameObject::GetLayer() const
    {
        return layer;
    }

    GameObject *GameObject::Create()
    {
        objects.push_back(std::make_unique<GameObject>());
        return objects.back().get();
    }

    GameObject *GameObject::CreatePrimitive(PrimitiveType type)
    {
        GameObject *g = Create();
        Mesh *mesh = nullptr;
        MeshRenderer *renderer = g->AddComponent<MeshRenderer>();
        renderer->SetCastShadows(true);
        renderer->SetReceiveShadows(true);

        switch(type)
        {
            case PrimitiveType::Capsule:
            {
                mesh = Resources::FindMesh(Constants::GetString(ConstantString::MeshCapsule));
                renderer->Add(mesh, std::make_shared<DiffuseMaterial>());
                break;
            }
            case PrimitiveType::Cube:
            {
                mesh = Resources::FindMesh(Constants::GetString(ConstantString::MeshCube));
                renderer->Add(mesh, std::make_shared<DiffuseMaterial>());
                break;
            }
            case PrimitiveType::Hemisphere:
            {
                mesh = Resources::FindMesh(Constants::GetString(ConstantString::MeshSphere));
                renderer->Add(mesh, std::make_shared<DiffuseMaterial>());
                break;
            }
            case PrimitiveType::Plane:
            {
                mesh = Resources::FindMesh(Constants::GetString(ConstantString::MeshPlane));
                renderer->Add(mesh, std::make_shared<DiffuseMaterial>());
                break;
            }
            case PrimitiveType::Quad:
            {
                mesh = Resources::FindMesh(Constants::GetString(ConstantString::MeshQuad));
                renderer->Add(mesh, std::make_shared<DiffuseMaterial>());
                break;
            }
            case PrimitiveType::Skybox:
            {
                mesh = Resources::FindMesh(Constants::GetString(ConstantString::MeshSkybox));
                renderer->Add(mesh, std::make_shared<DiffuseMaterial>());
                break;
            }
            case PrimitiveType::Sphere:
            {
                mesh = Resources::FindMesh(Constants::GetString(ConstantString::MeshSphere));
                renderer->Add(mesh, std::make_shared<DiffuseMaterial>());
                break;
            }
            default:
                break;
        }

        return g;
    }

    void GameObject::Destroy(GameObject *object)
    {
        if(!object)
            return;

        destroyQueue.push_back(object);
    }

    void GameObject::DestroyImmediate(GameObject *object)
    {
        for(size_t i = 0; i < object->components.size(); i++)
        {
            object->components[i]->OnDestroy();
        }

        auto children = object->GetTransform()->GetChildrenRecursive();

        for(size_t i = 0; i < children.size(); i++)
        {
            GameObject *currentObject = children[i]->GetGameObject();
            for(size_t j = 0; j < currentObject->components.size(); j++)
            {
                currentObject->components[j]->OnDestroy();
            }
        }

        for(size_t i = 0; i < children.size(); i++)
        {
            RemoveObject(children[i]->GetGameObject());
        }

        RemoveObject(object);
    }

    void GameObject::RemoveObject(GameObject *object)
    {
        if(!object)
            return;
        size_t index = 0;
        bool found = false;

        for(size_t i = 0; i < objects.size(); i++)
        {
            if(object->GetInstanceId() == objects[i]->GetInstanceId())
            {
                index = i;
                found = true;
                break;
            }
        }

        if(found)
        {
            object->GetTransform()->SetParent(nullptr);
            objects.erase(objects.begin() + index);
        }
    }

    void GameObject::DestroyAll()
    {
        objects.clear();
    }

    void GameObject::OnEndFrame()
    {
        if(destroyQueue.size() == 0)
            return;

        for(size_t i = 0; i < destroyQueue.size(); i++)
        {
            GameObject *object = destroyQueue[i];

            if(!object)
                continue;

            DestroyImmediate(object);
        }

        destroyQueue.clear();
    }
}