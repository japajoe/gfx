#include "GFX.hpp"
#include "Vehicle.hpp"

class GameManager : public GameBehaviour
{
private:
    void SpawnTerrain();
    void SpawnVehicle();
    Texture2D *LoadTexture(const std::string &filepath);
protected:
    void OnInitialize() override;
    void OnUpdate() override;
    void OnGUI() override;
};

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

void GameManager::OnInitialize()
{
    World::SetFogColor(Color(0x7c, 0x78, 0x7a, 255));

    auto camera = Camera::GetMain();
    camera->SetFarClippingPlane(10000);
    camera->GetTransform()->SetPosition(Vector3(0, 3, 0));

    GameObject::CreatePrimitive(PrimitiveType::ProceduralSkybox2);

    SpawnTerrain();
    SpawnVehicle();
}

void GameManager::OnUpdate()
{
    std::string fps = "FPS " +  std::to_string(Time::GetFPS());
    GUILayout::BeginFrame();
    GUILayout::Text(fps);
    GUILayout::EndFrame();

    if(Input::GetKeyDown(KeyCode::C))
    {
        Input::SetMouseCursor(!Input::IsCursorVisible());
    }
}

void GameManager::OnGUI()
{

}

void GameManager::SpawnTerrain()
{
    auto floor = GameObject::Create();
    auto mesh = std::make_shared<Mesh>(MeshGenerator::CreatePlane(Vector3(10000, 1, 10000)));
    auto renderer = floor->AddComponent<MeshRenderer>();
    renderer->Add(mesh, std::make_shared<DiffuseMaterial>());
    auto texture = LoadTexture("../res/Textures/Grass.jpg");
    auto material = floor->GetComponent<MeshRenderer>()->GetMaterial<DiffuseMaterial>(0);
    material->SetUVScale(Vector2(1000, 1000));
    material->SetDiffuseTexture(texture);
    auto collider = floor->AddComponent<MeshCollider>();
    collider->SetMesh(renderer->GetMesh(0));
    floor->AddComponent<Rigidbody>(0);
}

void GameManager::SpawnVehicle()
{
    ModelFlags flags = ModelFlags_Triangulate | ModelFlags_FlipUVs;
    auto model = ModelImporter::LoadFromFile("../res/Models/Mustang/Mustang.fbx", flags, Vector3f::One() * 0.01f);
    
    auto texture = LoadTexture("../res/Models/Mustang/Textures/AFRC_Tex_Col2.png");
    auto renderers = model->GetComponentsOfTypeInChildren<MeshRenderer>();
    for(size_t i = 0; i < renderers.size(); i++)
    {
        DiffuseMaterial *material = nullptr;
        size_t index = 0;
        while((material = renderers[i]->GetMaterial<DiffuseMaterial>(index++)) != nullptr)
        {
            material->SetDiffuseTexture(texture);
        }
    }

    auto vehicle = model->AddComponent<Vehicle>();
    VehicleSettings settings;
    settings.colliderPosition = Vector3(0, 0.9f, 0);
    settings.colliderSize = Vector3(1.6f, 1.2f, 4.8f);
    settings.mass = 1000.0f;
    settings.turnRadius = 10.8f;

    auto frontLeft = model->GetTransform()->FindChild("FrontLeftWheel");
    auto frontRight = model->GetTransform()->FindChild("FrontRightWheel");
    auto rearLeft = model->GetTransform()->FindChild("RearLeftWheel");
    auto rearRight = model->GetTransform()->FindChild("RearRightWheel");

    auto createWheelCollider = [this] (Transform *t) -> WheelInfo {
        GameObject *colliderObject = GameObject::Create();
        colliderObject->GetTransform()->SetPosition(t->GetPosition());
        colliderObject->GetTransform()->SetParent(t->GetRoot());
        auto collider = colliderObject->AddComponent<WheelCollider>();
        collider->SetWheelRadius(0.35f);
        collider->SetSpringStiffness(20000);
        collider->SetDamperStiffness(2000);
        collider->SetRestLength(0.12f);
        collider->SetDebugLines(true);
        WheelInfo wheel;
        wheel.collider = collider;
        wheel.visual = t;
        return wheel;
    };

    settings.frontLeft = createWheelCollider(frontLeft);
    settings.frontRight = createWheelCollider(frontRight);
    settings.rearLeft = createWheelCollider(rearLeft);
    settings.rearRight = createWheelCollider(rearRight);

    vehicle->Configure(settings);

    auto orbit = Camera::GetMain()->GetGameObject()->AddComponent<MouseOrbit>();
    orbit->SetTarget(model->GetTransform());
    orbit->zAxisDistance *= 0.25f;
}

Texture2D *GameManager::LoadTexture(const std::string &filepath)
{
    Image image(filepath);

    if(image.IsLoaded())
    {
        return Resources::AddTexture2D(filepath, Texture2D(&image));
    }
    return nullptr;
}