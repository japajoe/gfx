#ifndef GFX_MESHCOLLIDER_HPP
#define GFX_MESHCOLLIDER_HPP

#include "Collider.hpp"
#include "../../Graphics/Mesh.hpp"

namespace GFX
{
	class MeshCollider : public Collider
	{
	private:
		Mesh *mesh;
	public:
	    MeshCollider();
	    void SetMesh(Mesh *mesh);
	    Mesh *GetMesh() const;
	};
}

#endif