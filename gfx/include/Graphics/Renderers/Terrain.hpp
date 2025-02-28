#ifndef GFX_TERRAIN_HPP
#define GFX_TERRAIN_HPP

#include "Renderer.hpp"
#include "../Materials/Material.hpp"
#include "../Mesh.hpp"
#include "../Image.hpp"
#include "../Materials/TerrainMaterial.hpp"
#include <memory>

namespace GFX
{
    enum class HeightMapFilterMode
    {
        Bilinear,
        Point
    };

    enum class TerrainHeightMode
    {
        Overwrite,
        Additive
    };

    enum class TerrainFalloffMode
    {
        None,
        Linear
    };

    class Terrain : public Renderer
    {
    private:
        Mesh mesh;
        Mesh *pMesh;
        std::unique_ptr<TerrainMaterial> material;
        uint32_t width;
        uint32_t depth;
        float scale;
        float maxHeight;
        void Initialize();
    protected:
        void OnInitialize() override;
        void OnDestroy() override;
    public:
        Terrain();
        Terrain(uint32_t size, float scale, float maxHeight);
        void OnRender() override;
        void OnRender(Material *material, Camera *camera) override;
        void Update();
        void SetHeight(uint32_t x, uint32_t y, float height, TerrainHeightMode mode = TerrainHeightMode::Overwrite, bool update = false);
        void SetHeights(uint32_t x, uint32_t y, int radius, float height, TerrainFalloffMode falloff, TerrainHeightMode mode = TerrainHeightMode::Overwrite, bool update = false);
        void SetHeights(Image *heightmap, float minHeight, float maxHeight, HeightMapFilterMode filterMode);
        void SmoothHeights(uint32_t x, uint32_t y, int radius, bool update = false);
        bool WorldToTerrainPoint(const Vector3 &worldPoint, Vector2 &terrainPoint);
        float GetHeightAtPoint(uint32_t x, uint32_t y);
        float GetAverageHeightAtPointWithRadius(uint32_t x, uint32_t y, int radius);
        bool GetVertexAtPoint(uint32_t x, uint32_t y, Vector3 &position);
        uint32_t GetWidth() const;
        uint32_t GetDepth() const;
        void SetScale(float scale);
        float GetScale() const;
        Vector2 GetSize() const;
        void SetMaxHeight(float height);
        float GetMaxHeight() const;
        TerrainMaterial *GetMaterial() const;
        Mesh *GetMesh(size_t index) const override;
    };
}

#endif