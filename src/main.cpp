#include "Core/Application.hpp"
#include "Core/FirstPersonCamera.hpp"
#include "Core/Constants.hpp"
#include "Core/GameBehaviour.hpp"
#include "Core/GameObject.hpp"
#include "Core/Camera.hpp"
#include "Core/Input.hpp"
#include "Core/Resources.hpp"
#include "Core/Time.hpp"
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
    GameObject *cube = nullptr;
    Texture2D *textureBox = nullptr;
    Texture2D *textureGrass = nullptr;
protected:
    void OnInitialize() override
    {
        auto camera = Camera::GetMain();
        camera->SetFarClippingPlane(10000);
        camera->SetClearColor(Color::White());
        camera->GetTransform()->SetPosition(Vector3(0, 2, 10));
        camera->GetGameObject()->AddComponent<FirstPersonCamera>();
        
        auto skybox = GameObject::CreatePrimitive(PrimitiveType::ProceduralSkybox);

        Image imageBox("../res/Box.jpg");
        Image imageGrass("../res/Grass.jpg");

        textureBox = Resources::AddTexture2D("Box.jpg", Texture2D(&imageBox));
        textureGrass = Resources::AddTexture2D("Grass.jpg", Texture2D(&imageGrass));

        cube = GameObject::CreatePrimitive(PrimitiveType::Cube);
        cube->GetTransform()->SetPosition(Vector3(0, 2, 0));
        auto cubeMaterial = cube->GetComponent<MeshRenderer>()->GetMaterial<DiffuseMaterial>(0);
        cubeMaterial->SetDiffuseTexture(textureBox);

        auto plane = GameObject::CreatePrimitive(PrimitiveType::Plane);
        plane->GetTransform()->SetScale(Vector3(1000, 1, 1000));
        auto planeMaterial = plane->GetComponent<MeshRenderer>()->GetMaterial<DiffuseMaterial>(0);
        planeMaterial->SetDiffuseTexture(textureGrass);
        planeMaterial->SetUVScale(Vector2(200, 200));
        

    }
    
    void OnUpdate() override
    {
        if(Input::GetKeyDown(KeyCode::Escape))
        {
            Application::Quit();
        }

        if(Input::GetKeyDown(KeyCode::C))
        {
            Input::SetMouseCursor(!Input::IsCursorVisible());
        }

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