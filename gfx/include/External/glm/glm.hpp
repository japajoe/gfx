#ifndef GFX_GLM_HPP
#define GFX_GLM_HPP

#define GLM_ENABLE_EXPERIMENTAL
//#include "../../../libs/glm/glm/glm.hpp"
//#include "../../../libs/glm/glm/gtc/quaternion.hpp"
//#include "../../../libs/glm/glm/gtc/matrix_inverse.hpp"
//#include "../../../libs/glm/glm/gtc/matrix_transform.hpp"
//#include "../../../libs/glm/glm/gtc/type_ptr.hpp"
//#include "../../../libs/glm/glm/gtx/quaternion.hpp"
//#include "../../../libs/glm/glm/gtx/transform.hpp"
//#include "../../../libs/glm/glm/gtx/compatibility.hpp"
//#include "../../../libs/glm/glm/gtx/euler_angles.hpp"
//#include "../../../libs/glm/glm/gtx/matrix_decompose.hpp"

#include "glmhpp.hpp"
#include "gtc/quaternion.hpp"
#include "gtc/matrix_inverse.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtc/type_ptr.hpp"
#include "gtx/quaternion.hpp"
#include "gtx/transform.hpp"
#include "gtx/compatibility.hpp"
#include "gtx/euler_angles.hpp"
#include "gtx/matrix_decompose.hpp"

namespace GFX
{
	typedef glm::vec2 Vector2;
	typedef glm::vec3 Vector3;
	typedef glm::vec4 Vector4;
	typedef glm::quat Quaternion;
	typedef glm::mat3 Matrix3;
	typedef glm::mat4 Matrix4;
}

#endif