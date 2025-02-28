#ifndef GFX_JETBRAINSMONOREGULAR_HPP
#define GFX_JETBRAINSMONOREGULAR_HPP

#include <cstdint>
#include <cstdlib>

namespace GFX
{
	class JetBrainsMonoRegular
	{
	public:
        static const uint8_t *GetData();
        static size_t GetSize();
	};
}

#endif