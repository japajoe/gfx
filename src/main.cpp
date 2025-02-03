#include "Core/Application.hpp"
#include "Core/FirstPersonCamera.hpp"
#include "Core/GameBehaviour.hpp"
#include "Core/GameObject.hpp"
#include "Core/Camera.hpp"
#include "Core/Input.hpp"
#include "Core/Resources.hpp"
#include "Core/Time.hpp"
#include "Graphics/Graphics2D.hpp"
#include "Graphics/Renderers/MeshRenderer.hpp"
#include "Graphics/Materials/DiffuseMaterial.hpp"
#include "Audio/AudioSource.hpp"
#include "Audio/AudioClip.hpp"
#include "External/imgui/imgui.h"
#include "System/Numerics/Quaternion.hpp"

using namespace GFX;

class GameManager : public GameBehaviour
{
private:
    Font *font;
    GameObject *cube = nullptr;
protected:
    void OnInitialize() override
    {
        auto camera = Camera::GetMain();
        camera->SetFarClippingPlane(10000);
        camera->SetClearColor(Color::White());
        camera->GetTransform()->SetPosition(Vector3(0, 2, 10));
        camera->GetGameObject()->AddComponent<FirstPersonCamera>();
        
        font = Resources::FindFont("Default");

        cube = GameObject::CreatePrimitive(PrimitiveType::Cube);
        cube->GetTransform()->SetPosition(Vector3(0, 2, 0));

        auto plane = GameObject::CreatePrimitive(PrimitiveType::Plane);
        plane->GetTransform()->SetScale(Vector3(1000, 1, 1000));
        auto mat = plane->GetComponent<MeshRenderer>()->GetMaterial<DiffuseMaterial>(0);
        mat->SetDiffuseColor(Color::Green());
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

        if(!cube)
            return;

        float y = Time::GetTime();
        float z = Time::GetTime();
        auto rotation = Quaternionf::Euler(0, y, y);
        cube->GetTransform()->SetRotation(rotation);
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