#include "GameObject.hpp"
#include "Resources.hpp"
#include "Constants.hpp"
#include "../Graphics/Mesh.hpp"
#include "../Graphics/Texture2D.hpp"
#include "../Graphics/Materials/DiffuseMaterial.hpp"
#include "../Graphics/Materials/SkyboxMaterial.hpp"
#include "../Graphics/Materials/ProceduralSkyboxMaterial.hpp"
#include "../Graphics/Materials/ProceduralSkybox2Material.hpp"
#include "../Graphics/Materials/WaterMaterial.hpp"
#include "../Graphics/Renderers/MeshRenderer.hpp"
#include "../Graphics/Renderers/ParticleSystem.hpp"
#include "../Graphics/Renderers/Terrain.hpp"

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
        GameObject *g = objects.back().get();
        g->SetLayer(Layer_Default);
        return g;
    }

    GameObject *GameObject::CreatePrimitive(PrimitiveType type)
    {
        GameObject *g = Create();

        switch(type)
        {
            case PrimitiveType::Capsule:
            {
                auto mesh = Resources::FindMesh(Constants::GetString(ConstantString::MeshCapsule));
                auto renderer = g->AddComponent<MeshRenderer>();
                renderer->Add(mesh, std::make_shared<DiffuseMaterial>());
                break;
            }
            case PrimitiveType::Cube:
            {
                auto mesh = Resources::FindMesh(Constants::GetString(ConstantString::MeshCube));
                auto renderer = g->AddComponent<MeshRenderer>();
                renderer->Add(mesh, std::make_shared<DiffuseMaterial>());
                break;
            }
            case PrimitiveType::Cylinder:
            {
                auto mesh = Resources::FindMesh(Constants::GetString(ConstantString::MeshCylinder));
                auto renderer = g->AddComponent<MeshRenderer>();
                renderer->Add(mesh, std::make_shared<DiffuseMaterial>());
                break;
            }
            case PrimitiveType::Hemisphere:
            {
                auto mesh = Resources::FindMesh(Constants::GetString(ConstantString::MeshSphere));
                auto renderer = g->AddComponent<MeshRenderer>();
                renderer->Add(mesh, std::make_shared<DiffuseMaterial>());
                break;
            }
            case PrimitiveType::ParticleSystem:
            {
                g->SetLayer(Layer_Default | Layer_IgnoreCulling | Layer_IgnoreRaycast, true);
                auto renderer = g->AddComponent<ParticleSystem>();
                renderer->SetRenderOrder(1002);
                break;
            }
            case PrimitiveType::Plane:
            {
                auto mesh = Resources::FindMesh(Constants::GetString(ConstantString::MeshPlane));
                auto renderer = g->AddComponent<MeshRenderer>();
                renderer->Add(mesh, std::make_shared<DiffuseMaterial>());
                break;
            }
            case PrimitiveType::Quad:
            {
                auto mesh = Resources::FindMesh(Constants::GetString(ConstantString::MeshQuad));
                auto renderer = g->AddComponent<MeshRenderer>();
                renderer->Add(mesh, std::make_shared<DiffuseMaterial>());
                break;
            }
            case PrimitiveType::ProceduralSkybox1:
            {
                auto mesh = Resources::FindMesh(Constants::GetString(ConstantString::MeshSphere));
                auto renderer = g->AddComponent<MeshRenderer>();
                renderer->Add(mesh, std::make_shared<ProceduralSkyboxMaterial>());
                renderer->SetCastShadows(false);
                renderer->SetReceiveShadows(false);
                renderer->SetRenderOrder(999);
                auto settings = renderer->GetSettings(0);
                settings->cullFace = false;
                settings->depthTest = false;
                g->SetLayer(Layer_Default | Layer_IgnoreCulling | Layer_IgnoreRaycast, true);
                break;
            }
            case PrimitiveType::ProceduralSkybox2:
            {
                auto mesh = Resources::FindMesh(Constants::GetString(ConstantString::MeshCube));
                auto renderer = g->AddComponent<MeshRenderer>();
                renderer->Add(mesh, std::make_shared<ProceduralSkybox2Material>());
                renderer->SetCastShadows(false);
                renderer->SetReceiveShadows(false);
                renderer->SetRenderOrder(999);
                auto settings = renderer->GetSettings(0);
                settings->cullFace = false;
                settings->depthTest = false;
                g->SetLayer(Layer_Default | Layer_IgnoreCulling | Layer_IgnoreRaycast, true);
                break;
            }
            case PrimitiveType::Skybox:
            {
                g->SetLayer(Layer_Default | Layer_IgnoreCulling | Layer_IgnoreRaycast, true);
                auto mesh = Resources::FindMesh(Constants::GetString(ConstantString::MeshSkybox));
                auto renderer = g->AddComponent<MeshRenderer>();
                renderer->Add(mesh, std::make_shared<SkyboxMaterial>());
                renderer->SetCastShadows(false);
                renderer->SetReceiveShadows(false);
                renderer->SetRenderOrder(999);
                auto settings = renderer->GetSettings(0);
                settings->cullFace = false;
                settings->depthTest = false;
                break;
            }
            case PrimitiveType::Sphere:
            {
                auto mesh = Resources::FindMesh(Constants::GetString(ConstantString::MeshSphere));
                auto renderer = g->AddComponent<MeshRenderer>();
                renderer->Add(mesh, std::make_shared<DiffuseMaterial>());
                break;
            }
            case PrimitiveType::Terrain:
            {
                auto renderer = g->AddComponent<Terrain>();
                renderer->SetCastShadows(false);
                renderer->SetReceiveShadows(true);
                break;
            }
            case PrimitiveType::Water:
            {
                auto mesh = std::make_shared<Mesh>(MeshGenerator::CreateTerrain(128, 128, Vector3(1, 1, 1)));
                auto material = std::make_shared<WaterMaterial>();
                material->SetReceiveShadows(true);
                auto renderer = g->AddComponent<MeshRenderer>();
                renderer->Add(mesh, material);
                renderer->SetCastShadows(false);
                auto settings = renderer->GetSettings(0);
                settings->cullFace = true;
                settings->depthTest = true;
                settings->alphaBlend = true;
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