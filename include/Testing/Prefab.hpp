#ifndef PREFAB_HPP
#define PREFAB_HPP

#include "../GFX.hpp"

class Prefab
{
public:
	static GameObject *InstantiateTerrain(bool setHeights = true);
	static GameObject *InstantiateSkybox();
};

#endif