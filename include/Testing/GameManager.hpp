#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include "../GFX.hpp"

class GameManager : public GameBehaviour
{
private:
    Rigidbody *rb = nullptr;
    void CreateTerrain();
    void CreateBall();
    void CreateCube();
    Texture2D *LoadTexture(const std::string &filepath);
protected:
    void OnInitialize() override;
    void OnUpdate() override;
    void OnFixedUpdate() override;
    void OnGUI() override;
};

#endif