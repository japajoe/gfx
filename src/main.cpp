#include "GFX.hpp"
#include "GameManager.hpp"

int main(int argc, char **argv) 
{
    Application application("GFX", 800, 600, WindowFlags_VSync);
    
    application.loaded = [] () {
        auto g = GameObject::Create();
        g->AddComponent<GameManager>();
    };
    
    application.Run();
    return 0;
}