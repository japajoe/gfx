#ifndef GFX_COMPONENT_HPP
#define GFX_COMPONENT_HPP

#include "Object.hpp"

namespace GFX
{
    class GameObject;
    class Transform;

    class Component : public Object
    {
    friend class GameObject;
    public:
        Component();
        virtual ~Component();
        GameObject *GetGameObject() const;
        Transform *GetTransform() const;
    protected:
        virtual void OnInitialize();
        virtual void OnDestroy();
        virtual void OnActivate();
        virtual void OnDeactivate();
    private:
        GameObject *gameObject;
        Transform *transform;
    };
}

#endif