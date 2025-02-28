#ifndef GFX_DEJAVUSANSMONO_HPP
#define GFX_DEJAVUSANSMONO_HPP

#include <cstdint>
#include <cstdlib>

namespace GFX
{
	class DejaVuSansMono
	{
	public:
        static const uint8_t *GetData();
        static size_t GetSize();
	};
}

#endif