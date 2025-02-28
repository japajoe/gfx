#include "Component.hpp"
#include "GameObject.hpp"
#include "Transform.hpp"
#include <utility>

namespace GFX
{
    Component::Component() : Object()
    {
        gameObject = nullptr;
        transform = nullptr;
    }

    Component::~Component()
    {
        
    }

    void Component::OnInitialize()
    {

    }

    void Component::OnDestroy()
    {

    }

    void Component::OnActivate()
    {

    }
    
    void Component::OnDeactivate()
    {

    }

    GameObject *Component::GetGameObject() const
    {
        return gameObject;
    }

    Transform *Component::GetTransform() const
    {
        return transform;
    }
}