#ifndef GFX_DEPTHMATERIAL
#define GFX_DEPTHMATERIAL

#include "Material.hpp"

namespace GFX
{
	class DepthMaterial : public Material
	{
	private:
        int uModel;
        int uHasInstanceData;

		bool hasInstanceData;
	public:
		DepthMaterial();
		bool HasInstanceData() const;
		void SetHasInstanceData(bool hasInstanceData);
		void Use(Transform *transform, Camera *camera) override;
	};
}

#endif