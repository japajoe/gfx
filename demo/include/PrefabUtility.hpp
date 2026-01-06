#ifndef PREFABUTILITY_HPP
#define PREFABUTILITY_HPP

#include "GFX.hpp"
#include <vector>

class PrefabUtility
{
public:
	static Texture2D *CreateTexture(const std::string &name, const Color &color);
	static Texture2D *CreateSplatMap(const std::string &name, const Vector2 &size);
	static GameObject *CreateGround();
	static GameObject *CreateTerrain();
	static std::vector<GameObject*> CreateCubes();
	static GameObject *CreateCharacter();
	static GameObject *CreateBall();
};

#endif