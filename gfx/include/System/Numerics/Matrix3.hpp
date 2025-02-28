#ifndef GFX_MATRIX3_HPP
#define GFX_MATRIX3_HPP

#include "../../External/glm/glm.hpp"

namespace GFX
{
    class Matrix3f
    {
    public:
        static glm::mat3 Identity();
    };
}

#endif