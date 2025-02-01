#ifndef IMGUIMANAGER_HPP
#define IMGUIMANAGER_HPP

struct GLFWwindow;

namespace GFX
{
    class ImGuiManager
    {
    public:
        ImGuiManager();
        void Initialize(GLFWwindow *window);
        void Deinitialize();
        void BeginFrame();
        void EndFrame();
    private:
        GLFWwindow *window;
    };
}

#endif