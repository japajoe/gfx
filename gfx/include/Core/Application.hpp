#ifndef GFX_APPLICATION_HPP
#define GFX_APPLICATION_HPP

#include <string>
#include <cstdint>
#include <vector>
#include <functional>

struct GLFWwindow;

namespace GFX
{
    using LoadEventCallback = std::function<void(void)>;
    using NewFrameCallback = std::function<void(void)>;

    enum WindowFlags_
    {
        WindowFlags_None = 0,
        WindowFlags_VSync = 1 << 0,
        WindowFlags_Maximize = 1 << 1,
        WindowFlags_Fullscreen = 1 << 2
    };

    typedef int WindowFlags;

    struct Configuration
    {
        std::string title;
        uint32_t width;
        uint32_t height;
        WindowFlags flags;
        std::vector<uint8_t> inconData;
    };

    class Application
    {
    public:
        LoadEventCallback loaded;
        NewFrameCallback newFrame;
		~Application();
        Application(const Configuration &config);
        Application(const char *title, int width, int height, WindowFlags flags);
        Application(const Application &other);
        Application(Application &&other) noexcept;
        Application& operator=(const Application &other);
        Application& operator=(Application &&other) noexcept;
        void Run();
        void Close();
        static void Quit();
        static GLFWwindow *GetNativeWindow();
    private:
        GLFWwindow *pWindow;
        Configuration config;
        static Application *instance;
        void Dispose();
        void Initialize();
        void Deinitialize();
        void NewFrame();
        void EndFrame();
        static void OnError(int32_t error_code, const char *description);
        static void OnWindowResize(GLFWwindow *window, int32_t width, int32_t height);
        static void OnWindowMove(GLFWwindow *window, int32_t x, int32_t y);
        static void OnKeyPress(GLFWwindow *window, int32_t key, int32_t scancode, int32_t action, int32_t mods);
        static void OnCharPress(GLFWwindow *window, uint32_t codepoint);
        static void OnMouseButtonPress(GLFWwindow *window, int32_t button, int32_t action, int32_t mods);
        static void OnMouseScroll(GLFWwindow *window, double xoffset, double yoffset);
    };
}

#endif