#ifndef IMGUIMANAGER_HPP
#define IMGUIMANAGER_HPP

struct GLFWwindow;

class ImGuiManager
{
public:
    ImGuiManager(GLFWwindow *window);
    void Initialize();
    void Deinitialize();
    void BeginFrame();
    void EndFrame();
private:
    GLFWwindow *window;
};

#endif