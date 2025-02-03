#ifndef GFX_CONSTANTS_HPP
#define GFX_CONSTANTS_HPP

#include <cstdint>
#include <string>

namespace GFX
{
	enum UniformBindingIndex : uint32_t
	{
		UniformBindingIndex_Camera = 0,
		UniformBindingIndex_Lights = 1,
		UniformBindingIndex_World = 2,
		UniformBindingIndex_Shadows = 3
	};
}

#endif