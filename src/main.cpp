#include "Core/Application.hpp"
#include "Core/FirstPersonCamera.hpp"
#include "Core/Constants.hpp"
#include "Core/GameBehaviour.hpp"
#include "Core/GameObject.hpp"
#include "Core/Camera.hpp"
#include "Core/Input.hpp"
#include "Core/Resources.hpp"
#include "Core/Time.hpp"
#include "Graphics/GUI.hpp"
#include "Graphics/Renderers/MeshRenderer.hpp"
#include "Graphics/Materials/DiffuseMaterial.hpp"
#include "Graphics/Materials/SkyboxMaterial.hpp"
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
        
        font = Resources::FindFont(Constants::GetString(ConstantString::FontDefault));

        auto skybox = GameObject::CreatePrimitive(PrimitiveType::ProceduralSkybox);

        cube = GameObject::CreatePrimitive(PrimitiveType::Cube);
        cube->GetTransform()->SetPosition(Vector3(0, 2, 0));

        auto plane = GameObject::CreatePrimitive(PrimitiveType::Plane);
        plane->GetTransform()->SetScale(Vector3(1000, 1, 1000));
        auto mat = plane->GetComponent<MeshRenderer>()->GetMaterial<DiffuseMaterial>(0);
        mat->SetDiffuseColor(Color::Green());

    }
    
    void OnUpdate() override
    {
        if(font == nullptr)
            return;

        GUI::BeginFrame();
        
        if(GUI::Button(Rectangle(10, 10, 100, 20), "Hello world"))
        {
            printf("Button clicked\n");
        }

        GUI::EndFrame();

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