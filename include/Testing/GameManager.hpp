#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include "../GFX.hpp"
#include "Vehicle.hpp"

class GameManager : public GameBehaviour
{
private:
	Vehicle *vehicle = nullptr;
    void SpawnTerrain();
    void SpawnVehicle();
protected:
    void OnInitialize() override;
    void OnUpdate() override;
    void OnGUI() override;
};

#endif