#ifndef GAMEMANAGER_HPP
#define GAMEMANAGER_HPP

#include "../GFX.hpp"

class GameManager : public GameBehaviour
{
private:
    GameObject *cube = nullptr;
    Font *font = nullptr;
    size_t totalResourceCount = 0;
    size_t currentResourceCount = 0;
    void LoadResources();
protected:
    void OnInitialize() override;
    void OnUpdate() override;
    void OnGUI() override;
    void OnResourceBatchLoadedAsync(const ResourceBatch &resourceBatch) override;
    void OnLoadingComplete();
};

#endif