#include "Core/Application.hpp"
#include "Graphics/Graphics2D.hpp"

using namespace GFX;

void OnApplicationLoaded();
void OnNewFrame();

Font font;

int main(int argc, char **argv) 
{
    Application application("Test", 512, 512, WindowFlags_VSync);
    application.loaded = OnApplicationLoaded;
    application.newFrame = OnNewFrame;
    application.Run();
    return 0;
}

void OnApplicationLoaded()
{
    if(font.LoadFromFile("BarlowCondensed-Regular.ttf", 64, FontRenderMethod::SDF))
    {
        font.GenerateTexture();
    }
}

void OnNewFrame()
{
    std::string text = "Hello world";
    Graphics2D::AddText(Vector2(10, 10), &font, text, 48.0f, Color(1.0f, 0.0f, 0.0f, 1.0f), false);
}