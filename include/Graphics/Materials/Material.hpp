#ifndef GFX_MATERIAL_HPP
#define GFX_MATERIAL_HPP

#include "../Shader.hpp"
#include "../Texture2D.hpp"
#include "../Texture3D.hpp"
#include "../TextureCubeMap.hpp"
#include "../Color.hpp"
#include "../../Core/Camera.hpp"
#include "../../Core/Transform.hpp"
#include "../../System/Numerics/Vector2.hpp"
#include "../../System/Numerics/Vector3.hpp"
#include "../../System/Numerics/Vector4.hpp"
#include "../../System/Numerics/Matrix3.hpp"
#include "../../System/Numerics/Matrix4.hpp"

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