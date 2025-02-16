#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include "../GFX.hpp"
#include <vector>

class GameManager : public GameBehaviour
{
private:
    Rigidbody *rb = nullptr;
    std::vector<Collider*> colliders;
    void CreateTerrain();
    void CreateBall();
    void CreateCube();
    Texture2D *LoadTexture(const std::string &filepath);
protected:
    void OnInitialize() override;
    void OnUpdate() override;
    void OnFixedUpdate() override;
    void OnGUI() override;
    void OnPostProcess(uint32_t shaderId) override;
};

#endif