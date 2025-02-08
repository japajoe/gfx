#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include "../GFX.hpp"

class GameManager : public GameBehaviour
{
protected:
    void OnInitialize() override;
    void OnUpdate() override;
    void OnGUI() override;
    void UpdateTexture();
};

#endif