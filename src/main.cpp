#include "GFX.hpp"
#include "Testing/GameManager.hpp"

void CreateAssetPack();

int main(int argc, char **argv) 
{
    Application application("Test", 512, 512, WindowFlags_VSync);
    
    application.loaded = [] () {
        auto g = GameObject::Create();
        g->AddComponent<GameManager>();        
    };
    
    application.Run();
    return 0;
}

void CreateAssetPack()
{
    AssetPack assetPack;
    assetPack.AddFile("Resources/Audio/unhappy-drone-67284.wav");
    assetPack.AddFile("Resources/Audio/click1.mp3");
    assetPack.AddFile("Resources/Audio/click2.mp3");
    assetPack.AddFile("Resources/Audio/Fire.wav");
    assetPack.AddFile("Resources/Fonts/SF Sports Night.ttf");
    assetPack.AddFile("Resources/Shaders/BasicV.glsl");
    assetPack.AddFile("Resources/Shaders/FireF.glsl");
    assetPack.AddFile("Resources/Textures/Box.jpg");
    assetPack.AddFile("Resources/Textures/Grass.jpg");
    assetPack.AddFile("Resources/Textures/billboardgrass0002.png");
    assetPack.AddFile("Resources/Textures/RedFlower.png");
    assetPack.AddFile("Resources/Textures/YellowFlower.png");
    assetPack.AddFile("Resources/Textures/coast_sand_rocks_02_diff_1k.jpg");
    assetPack.AddFile("Resources/Textures/forrest_ground_01_diff_1k.jpg");
    assetPack.AddFile("Resources/Textures/Mud.png");
    assetPack.AddFile("Resources/Textures/Splatmap.jpg");
    assetPack.AddFile("Resources/Textures/logo.png");
    assetPack.AddFile("Resources/Textures/smoke_04.png");
    assetPack.AddFile("Resources/Textures/Water.jpg");
    assetPack.AddFile("Resources/Textures/Skyboxes/Sahara/right.png");
    assetPack.AddFile("Resources/Textures/Skyboxes/Sahara/left.png");
    assetPack.AddFile("Resources/Textures/Skyboxes/Sahara/bottom.png");
    assetPack.AddFile("Resources/Textures/Skyboxes/Sahara/top.png");
    assetPack.AddFile("Resources/Textures/Skyboxes/Sahara/front.png");
    assetPack.AddFile("Resources/Textures/Skyboxes/Sahara/back.png");
    assetPack.AddFile("Resources/Textures/Skyboxes/Sahara/back.png");

    if(assetPack.Save("../res/assets.dat", "assets.dat"))
    {
        printf("Asset pack saved\n");
    }
}