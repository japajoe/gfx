#ifndef GFX_HINGEJOINT_HPP
#define GFX_HINGEJOINT_HPP

#include "Joint.hpp"

namespace GFX
{
	class HingeJoint : public Joint
	{
	protected:
		void OnInitialize() override;
	};
}

#endif