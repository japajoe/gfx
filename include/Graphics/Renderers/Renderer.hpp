#ifndef GFX_RENDERER_HPP
#define GFX_RENDERER_HPP

#include "../../Core/Component.hpp"
#include "../../Core/Camera.hpp"
#include "../../External/glad/glad.h"
#include "../Materials/Material.hpp"
#include <memory>
#include <type_traits>
#include <cstdint>
#include <cstdlib>

namespace GFX
{
    struct RenderSettings
    {
        bool wireframe;
        bool depthTest;
        bool cullFace;
        bool alphaBlend;
        GLenum depthFunc;
        RenderSettings();
    };

    enum class RendererType
    {
        Batch,
        Mesh,
        Terrain
    };

    class Mesh;

    class Renderer : public Component
    {
    protected:
        bool castShadows;
        bool receiveShadows;
        uint32_t renderOrder;
        RendererType type;
    public:
        Renderer();
        virtual void OnRender() = 0;
        virtual void OnRender(Material *material, Camera *camera) = 0;
        virtual Mesh *GetMesh(size_t index) const;
        void SetCastShadows(bool castShadows);
        bool GetCastShadows() const;
        void SetReceiveShadows(bool receiveShadows);
        bool GetReceiveShadows() const;
        void SetRenderOrder(uint32_t order);
        uint32_t GetRenderOrder() const;
        RendererType GetType() const;
    };

    struct CompareRenderer 
    {
        bool operator()(const Renderer *a, const Renderer *b) const 
        {
            return a->GetRenderOrder() > b->GetRenderOrder(); // Lower queuePosition has higher priority
        }
    };
}

#endif