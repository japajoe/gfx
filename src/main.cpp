#include "Core/Application.hpp"
#include "Core/GameBehaviour.hpp"
#include "Core/GameObject.hpp"
#include "Core/Camera.hpp"
#include "Core/Input.hpp"
#include "Core/Resources.hpp"
#include "Graphics/Graphics2D.hpp"
#include "Audio/AudioSource.hpp"
#include "Audio/AudioClip.hpp"
#include "External/imgui/imgui.h"

using namespace GFX;

class GameManager : public GameBehaviour
{
private:
    Font *font;
protected:
    void OnInitialize() override
    {
        auto camera = Camera::GetMain();
        camera->SetClearColor(Color::LightGray());
        font = Resources::FindFont("Default");
    }

    Vector2 CalculateTextSize(const std::string &text, float fontSize)
    {
        Vector2 bounds;
        font->CalculateBounds(text.c_str(), text.length(), fontSize, bounds.x, bounds.y);
        return bounds;
    }

    Vector2 CalculateCenteredPosition(const Rectangle &rect, const Vector2 &size)
    {
        return Vector2((rect.width - size.x) * 0.5f, (rect.height - size.y) * 0.5f);
    }

    void RenderText(const Vector2 &position, const std::string &text)
    {
        float fontSize = 16;
        Color bgColor(0.2f, 0.2f, 0.2f, 1.0f);
        Color fontColor = Color::RayWhite();

        Rectangle rect(position.x, position.y, 200, 32);
        Vector2 textSize = CalculateTextSize(text, fontSize);
        rect.width = textSize.x + 10;
        rect.height = textSize.y + 10;
        Vector2 size = rect.GetSizeFromRectangle();
        Vector2 textPosition = position + CalculateCenteredPosition(rect, textSize);

        Graphics2D::AddRectangleRounded(position, size, 0, 5, bgColor);
        Graphics2D::AddText(textPosition, font, text, fontSize, fontColor, true);
    }
    
    void OnUpdate() override
    {
        if(font == nullptr)
            return;

        RenderText(Vector2(10, 10), "Hello world");

        if(Input::GetKeyDown(KeyCode::Escape))
        {
            Application::Quit();
        }
    }

    void OnGUI() override
    {

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