#ifndef GFX_RESOURCE_HPP
#define GFX_RESOURCE_HPP

#include <string>
#include <vector>

namespace GFX
{
    enum class ResourceType
    {
        AudioClip,
        Texture2D,
        Texture2DArray,
        TextureCubeMap,
        Shader,
        Font,
        Blob
    };

    enum class ResourceLoadResult
    {
        Ok,
        Error
    };

    struct Resource
    {
        ResourceType type;
		ResourceLoadResult result;
        std::string name;
        std::vector<uint8_t> data;
        uint32_t width;
        uint32_t height;
        uint32_t channels;
    };

    struct ResourceBatch
    {
        ResourceType type;
        std::vector<Resource> resources;
    };
}

#endif