#include "GFX.hpp"
#include "PhysicsDemo.hpp"

void OnLoaded();

int main(int argc, char **argv)
{
	Application application("Test", 800, 600, WindowFlags_VSync);
	application.loaded = [] () {
		OnLoaded();
	};
	application.Run();

	return 0;
}

void OnLoaded()
{
	auto g = GameObject::Create();
	g->AddComponent<PhysicsDemo>();
}
