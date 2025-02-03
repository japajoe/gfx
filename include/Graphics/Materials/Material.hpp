#ifndef GFX_MATERIAL_HPP
#define GFX_MATERIAL_HPP

#include "../Shader.hpp"
#include "../../Core/Camera.hpp"
#include "../../Core/Transform.hpp"

namespace GFX
{
	class Material
	{
	public:
		Material();
		virtual void Use(Transform *transform, Camera *camera);
		Shader *GetShader() const;
	protected:
		Shader *shader;
	};
}

#endif