#include "GFX.hpp"

class GameManager : public GameBehaviour
{
private:
    GameObject *cube = nullptr;
    Font *font = nullptr;
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

        auto textureBox = Resources::AddTexture2D("Box.jpg", Texture2D(&imageBox));
        auto textureGrass = Resources::AddTexture2D("Grass.jpg", Texture2D(&imageGrass));

        cube = GameObject::CreatePrimitive(PrimitiveType::Cube);
        cube->GetTransform()->SetPosition(Vector3(0, 2, 0));
        auto cubeMaterial = cube->GetComponent<MeshRenderer>()->GetMaterial<DiffuseMaterial>(0);
        cubeMaterial->SetDiffuseTexture(textureBox);

        auto plane = GameObject::CreatePrimitive(PrimitiveType::Plane);
        plane->GetTransform()->SetScale(Vector3(1000, 1, 1000));
        auto planeMaterial = plane->GetComponent<MeshRenderer>()->GetMaterial<DiffuseMaterial>(0);
        planeMaterial->SetDiffuseTexture(textureGrass);
        planeMaterial->SetUVScale(Vector2(200, 200));
        
        font = Resources::FindFont(Constants::GetString(ConstantString::FontDefault));
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

        std::string fps = std::to_string(Time::GetFPS());

        Graphics2D::AddText(Vector2(10, 10), font, fps, 22, Color::White(), false);
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