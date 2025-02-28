#include "Terrain.hpp"
#include "../../Core/GameObject.hpp"
#include "../../Core/Constants.hpp"
#include "../../Core/Input.hpp"
#include "../../Core/Time.hpp"
#include "../../Core/Resources.hpp"
#include "../../External/glm/glm.hpp"
#include "../Texture2D.hpp"
#include "../GL.hpp"
#include "../Graphics.hpp"

namespace GFX
{
    static constexpr uint32_t GetNextPowerOfTwo(uint32_t value) 
    {
        value--;
        value |= value >> 1;
        value |= value >> 2;
        value |= value >> 4;
        value |= value >> 8;
        value |= value >> 16;
        value++;
        return value;
    }

    Terrain::Terrain() : Renderer()
    {
        width = 128;
        depth = 128;
        scale = 10.0f;
        maxHeight = 128.0f;
        pMesh = &mesh;
        type = RendererType::Terrain;
        SetName("Terrain");
    }

    Terrain::Terrain(uint32_t size, float scale, float maxHeight)
    {
        if(size < 2)
            size = 2;

        size = GetNextPowerOfTwo(size);
        
        this->width = size;
        this->depth = size;
        this->scale = scale;
        this->maxHeight = maxHeight;
        this->pMesh = &mesh;
        this->type = RendererType::Terrain;
        SetName("Terrain");
    }

    void Terrain::OnInitialize()
    {
        material = std::make_unique<TerrainMaterial>();
        auto texture = Resources::FindTexture2D(Constants::GetString(ConstantString::TextureDefault));

        material->SetSplatMap(texture);
        material->SetTexture1(texture);
        material->SetTexture2(texture);
        material->SetTexture3(texture);
        material->SetTexture4(texture);

        mesh = MeshGenerator::CreateTerrain(width, depth, Vector3(scale, scale, scale));
        mesh.Generate();

        Graphics::Add(this);
    }

    void Terrain::OnDestroy()
    {
        Graphics::Remove(this);
        mesh.Delete();
    }

    void Terrain::OnRender()
    {
        if(!GetGameObject()->GetIsActive())
            return;

        if(mesh.GetVAO()->GetId() == 0)
            return;
        
        Camera *camera = Camera::GetMain();
        Transform *transform = GetTransform();

        if(!camera || !transform || !material)
            return;

        if(!material->GetShader())
            return;

        material->Use(transform, camera);

        mesh.GetVAO()->Bind();

        GL::DepthTest(true);
        GL::CullFace(true);
        GL::BlendMode(false);

        if(mesh.GetEBO()->GetId() > 0)
            glDrawElements(GL_TRIANGLES, mesh.GetIndicesCount(), GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0, mesh.GetVerticesCount());
        
        mesh.GetVAO()->Unbind();
    }

    void Terrain::OnRender(Material *material, Camera *camera)
    {
        if(!GetGameObject()->GetIsActive())
            return;
        
        if(mesh.GetVAO()->GetId() == 0)
            return;
        
        Transform *transform = GetTransform();

        if(!camera || !transform || !material)
            return;

        if(!material->GetShader())
            return;

        material->Use(transform, camera);

        mesh.GetVAO()->Bind();

        GL::DepthMask(true);
        GL::CullFace(true);
        GL::BlendMode(false);

        if(mesh.GetEBO()->GetId() > 0)
            glDrawElements(GL_TRIANGLES, mesh.GetIndicesCount(), GL_UNSIGNED_INT, 0);
        else
            glDrawArrays(GL_TRIANGLES, 0, mesh.GetVerticesCount());
        
        mesh.GetVAO()->Unbind();
    }


    void Terrain::Update()
    {
        mesh.Generate();
    }

    void Terrain::SetHeight(uint32_t x, uint32_t y, float height, TerrainHeightMode mode, bool update)
    {
        auto &vertices = mesh.GetVertices();
        int vertsPerRow = width + 1;
        int index = (y * vertsPerRow) + x;
        if(index < vertices.size())
        {
            if(mode == TerrainHeightMode::Overwrite)
            {
                if(height > maxHeight)
                    height = maxHeight;
                vertices[index].position.y = height;
            }
            else
            {
                float newHeight = vertices[index].position.y + height;
                if(newHeight > maxHeight)
                    newHeight = maxHeight;
                vertices[index].position.y = newHeight;
            }
            if(update)
                Update();
        }
    }

    static float InverseLerp(float start, float end, float value)
    {
        return (value - start) / (end - start);
    }

    void Terrain::SetHeights(uint32_t x, uint32_t y, int radius, float height, TerrainFalloffMode falloff, TerrainHeightMode mode, bool update)
    {
        int radius2 = static_cast<int>(glm::pow(radius, 2));

        for (int j = -radius; j <= radius; j++)
        {
            int j2 = static_cast<int>(glm::pow(j, 2));

            for (int k = -radius; k <= radius; k++)
            {
                int k2 = static_cast<int>(glm::pow(k, 2));

                if(j2 + k2 <= radius2)
                {
                    float distance = static_cast<float>(j2 + k2);
                    float factor = falloff == TerrainFalloffMode::None ? 1.0f : (1.1f - InverseLerp(0, radius2, distance));

                    int px = j + x;
                    int py = k + y;
                    SetHeight(px, py, height * factor, mode);
                }
            }
        }

        if(update)
        {
            Update();
        }
    }

    void Terrain::SetHeights(Image *heightmap, float minHeight, float maxHeight, HeightMapFilterMode filterMode)
    {
        auto pixels = heightmap->GetData();
        size_t dataSize = heightmap->GetDataSize();

        uint32_t w = heightmap->GetWidth();
        uint32_t h = heightmap->GetHeight();
        uint32_t c = heightmap->GetChannels();
        uint32_t w2 = this->width + 1;

        Color *mapColors = new Color[w * h];

        size_t index = 0;
        for(size_t i = 0; i < dataSize; i+=c)
        {
            mapColors[index++] = Color(pixels[i+0], pixels[i+1], pixels[i+2], 255);
        }

        if (w2 != w || h != w)
        {
            Color *map = new Color[w2 * w2];

            // Resize using nearest-neighbor scaling if texture has no filtering
            if (filterMode == HeightMapFilterMode::Point)
            {
                float dx = static_cast<float>(w) / static_cast<float>(w2);
                float dy = static_cast<float>(h) / static_cast<float>(w2);

                for (int y = 0; y < w2; y++)
                {

                    int thisY = static_cast<int>(dy * y) * w;
                    int yw = y * w2;
                    for (int x = 0; x < w2; x++)
                    {
                        map[yw + x] = mapColors[static_cast<int>(thisY + dx * x)];
                    }
                }
            }
            // Otherwise resize using bilinear filtering
            else
            {
                float ratioX = 1.0f / (static_cast<float>(w2) / (w - 1));
                float ratioY = 1.0f / (static_cast<float>(w2) / (h - 1));
                for (int y = 0; y < w2; y++)
                {
                    float yy = glm::floor(y * ratioY);
                    float y1 = yy * w;
                    float y2 = (yy + 1) * w;
                    float yw = y * w2;

                    for (int x = 0; x < w2; x++)
                    {
                        float xx = glm::floor(x * ratioX);

                        Color bl = mapColors[static_cast<int>(y1 + xx)];
                        Color br = mapColors[static_cast<int>(y1 + xx + 1)];
                        Color tl = mapColors[static_cast<int>(y2 + xx)];
                        Color tr = mapColors[static_cast<int>(y2 + xx + 1)];

                        float xLerp = x * ratioX - xx;
                        map[static_cast<int>(yw + x)] = Color::Lerp(Color::Lerp(bl, br, xLerp), Color::Lerp(tl, tr, xLerp), y * ratioY - yy);
                    }
                }
            }

            // Assign texture data to heightmap
            for (int y = 0; y < depth+1; y++)
            {
                for (int x = 0; x < width+1; x++)
                {
                    Color col = map[y * w2 + x];
                    float grayscale = Color::ToGrayScale(col).r;
                    float newHeight = minHeight + ((maxHeight - minHeight) * grayscale);                    
                    SetHeight(x, y, newHeight);
                }
            }

            delete[] map;
        }
        else
        {
            // No resize is needed
            // Assign texture data to heightmap
            for (int y = 0; y < depth+1; y++)
            {
                for (int x = 0; x < width+1; x++)
                {
                    Color col = mapColors[y * w2 + x];
                    float grayscale = Color::ToGrayScale(col).r;
                    float newHeight = minHeight + ((maxHeight - minHeight) * grayscale);                    
                    SetHeight(x, y, newHeight);
                }
            }
        }

        delete[] mapColors;
    }

    void Terrain::SmoothHeights(uint32_t x, uint32_t y, int radius, bool update)
    {
        int radius2 = static_cast<int>(glm::pow(radius, 2));
        float averageHeight = GetAverageHeightAtPointWithRadius(x, y, radius);

        for (int j = -radius; j <= radius; j++)
        {
            int j2 = static_cast<int>(glm::pow(j, 2));

            for (int k = -radius; k <= radius; k++)
            {
                int k2 = static_cast<int>(glm::pow(k, 2));

                if(j2 + k2 <= radius2)
                {
                    int px = j + x;
                    int py = k + y;
                    float distanceSquared =  glm::length2(Vector2(px, py) - Vector2(x, y));
                    float influence = 1.0f - (distanceSquared / radius2);
                    influence = glm::clamp(influence, 0.0f, 1.0f);
                    
                    float distance = static_cast<float>(j2 + k2);
                    //float factor = 1.1f - InverseLerp(0, radius2, distance);
                    float factor = 0.1f + InverseLerp(0, radius2, distance);


                    SetHeight(px, py, averageHeight * factor, TerrainHeightMode::Overwrite);
                }
            }
        }

        if(update)
        {
            Update();
        }
    }

    bool Terrain::WorldToTerrainPoint(const Vector3 &worldPoint, Vector2 &terrainPoint)
    {
        Transform *transform = GetTransform();

        float offsetX = worldPoint.x - transform->GetPosition().x;
        float offsetZ = worldPoint.z - transform->GetPosition().z;

        offsetZ *= -1.0f; //Flip direction because -z is forward

        uint32_t width = GetWidth();
        uint32_t depth = GetDepth();

        float terrainMinX = 0.0f;
        float terrainMaxX = width * GetScale();

        float terrainMinZ = 0.0f;
        float terrainMaxZ = depth * GetScale();

        int tileX = (int)glm::floor(InverseLerp(terrainMinX, terrainMaxX, offsetX) * width);
        int tileZ = (int)glm::floor(InverseLerp(terrainMinZ, terrainMaxZ, offsetZ) * depth);

        if(tileX >= width || tileZ >= depth)
            return false;
        if(tileX < 0 || tileZ < 0)
            return false;
        
        terrainPoint.x = tileX;
        terrainPoint.y = tileZ;
        return true;
    }

    float Terrain::GetHeightAtPoint(uint32_t x, uint32_t y)
    {
        int vertsPerRow = width + 1;
        int index = (y * vertsPerRow) + x;
        auto &vertices = mesh.GetVertices();

        if(index >= vertices.size())
            return 0.0f;

        return vertices[index].position.y;
    }

    float Terrain::GetAverageHeightAtPointWithRadius(uint32_t x, uint32_t y, int radius)
    {
        float averageHeight = 0;
        int numTiles = 0;
        int radius2 = static_cast<int>(glm::pow(radius, 2));

        for (int j = -radius; j <= radius; j++)
        {
            int j2 = static_cast<int>(glm::pow(j, 2));

            for (int k = -radius; k <= radius; k++)
            {
                int k2 = static_cast<int>(glm::pow(k, 2));

                if(j2 + k2 <= radius2)
                {
                    float distance = static_cast<float>(j2 + k2);
                    int px = j + x;
                    int py = k + y;
                    averageHeight += GetHeightAtPoint(px, py);
                    numTiles++;
                }
            }
        }

        if(numTiles > 0)
        {
            averageHeight /= static_cast<float>(numTiles);
        }

        return averageHeight;
    }

    bool Terrain::GetVertexAtPoint(uint32_t x, uint32_t y, Vector3 &position)
    {
        int vertsPerRow = width + 1;
        int index = (y * vertsPerRow) + x;
        auto &vertices = mesh.GetVertices();

        if(index >= vertices.size())
            return false;

        position = vertices[index].position;
        return true;
    }

    uint32_t Terrain::GetWidth() const
    {
        return width;
    }

    uint32_t Terrain::GetDepth() const
    {
        return depth;
    }

    Vector2 Terrain::GetSize() const
    {
        return Vector2(width * scale, depth * scale);
    }

    void Terrain::SetScale(float scale)
    {
        auto &vertices = mesh.GetVertices();

        float normalizedScale = 1.0f / this->scale;
        this->scale = scale;

        for(size_t i = 0; i < vertices.size(); i++)
        {
            //Normalize scale
            vertices[i].position *= normalizedScale;
            vertices[i].position *= scale;
        }

        mesh.RecalculateNormals();
        mesh.Generate();
    }

    float Terrain::GetScale() const
    {
        return scale;
    }

    void Terrain::SetMaxHeight(float height)
    {
        this->maxHeight = height;
    }

    float Terrain::GetMaxHeight() const
    {
        return maxHeight;
    }

    TerrainMaterial *Terrain::GetMaterial() const
    {
        return material.get();
    }

    Mesh *Terrain::GetMesh(size_t index) const
    {
        if(index == 0)
            return pMesh;
        return nullptr;
    }
}