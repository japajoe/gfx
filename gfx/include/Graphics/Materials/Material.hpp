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
#include <string>

namespace GFX
{
	class Material
	{
	public:
		Material();
		virtual void Use(Transform *transform, Camera *camera);
		Shader *GetShader() const;
		void SetName(const std::string &name);
		std::string GetName() const;
	protected:
		Shader *shader;
		std::string name;
	};
}

#endif