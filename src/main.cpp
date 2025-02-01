#include "Core/Application.hpp"

using namespace GFX;

int main(int argc, char **argv) 
{
    Application application("Test", 512, 512, WindowFlags_VSync);
    application.Run();
    return 0;
}
