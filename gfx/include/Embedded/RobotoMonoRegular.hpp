#ifndef GFX_ROBOTOMONOREGULAR_HPP
#define GFX_ROBOTOMONOREGULAR_HPP

#include <cstdint>
#include <cstdlib>

namespace GFX
{
	class RobotoMonoRegular
	{
	public:
        static const uint8_t *GetData();
        static size_t GetSize();
	};
}

#endif