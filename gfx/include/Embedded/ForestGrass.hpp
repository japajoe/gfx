#ifndef GFX_FORESTGRASS_HPP
#define GFX_FORESTGRASS_HPP

#include <cstdint>
#include <cstdlib>

namespace GFX
{
	class ForestGrass
	{
	public:
        static const uint8_t *GetData();
        static size_t GetSize();
	};
}

#endif