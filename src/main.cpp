#include "Core/Application.hpp"
#include "Core/GameBehaviour.hpp"
#include "Core/GameObject.hpp"
#include "Graphics/Graphics2D.hpp"
#include "External/imgui/imgui.h"

using namespace GFX;

class GameManager : public GameBehaviour
{
private:
    Font font;
protected:
    void OnInitialize() override
    {
        if(font.LoadFromFile("BarlowCondensed-Regular.ttf", 64, FontRenderMethod::SDF))
        {
            font.GenerateTexture();
        }
    }
    
    void OnUpdate() override
    {
        std::string text = "Hello {00FF00FF}world";
        Graphics2D::AddText(Vector2(10, 10), &font, text, 48.0f, Color(1.0f, 0.0f, 0.0f, 1.0f), true);
    }

    void OnGUI() override
    {
        ImGui::Begin("Test");

        ImGui::End();
    }
};

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