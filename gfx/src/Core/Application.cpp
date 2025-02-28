#include "Application.hpp"
#include "../External/glad/glad.h"
#include "../../libs/glfw/include/GLFW/glfw3.h"
#include "../Graphics/Graphics.hpp"
#include "../Graphics/Image.hpp"
#include "../Audio/Audio.hpp"
#include "../Physics/Physics.hpp"
#include "Input.hpp"
#include "Time.hpp"
#include "GameBehaviour.hpp"
#include "Resources.hpp"
#include <iostream>

namespace GFX
{
    Application *Application::instance = nullptr;

	Application::~Application()
	{
		Dispose();
	}

    Application::Application(const Configuration &config)
    {
        this->config = config;
        this->pWindow = nullptr;
        this->loaded = nullptr;
        this->newFrame = nullptr;
        instance = this;
    }

    Application::Application(const char *title, int width, int height, WindowFlags flags)
    {
        this->config.title = title;
        this->config.width = width;
        this->config.height = height;
        this->config.flags = flags;
        this->pWindow = nullptr;
        this->loaded = nullptr;
        this->newFrame = nullptr;
        instance = this;
    }

	Application::Application(const Application &other)
	{
		this->config = other.config;
		this->pWindow = other.pWindow;
		this->loaded = other.loaded;
        this->newFrame = other.newFrame;
	}

	Application::Application(Application &&other) noexcept
	{
		this->config = std::move(other.config);
		this->pWindow = std::exchange(other.pWindow, nullptr);
		this->loaded = std::move(other.loaded);
        this->newFrame = std::move(other.newFrame);
	}

	Application& Application::operator=(const Application &other)
	{
		if(this != &other)
		{
			this->config = other.config;
			this->pWindow = other.pWindow;
			this->loaded = other.loaded;
            this->newFrame = other.newFrame;
		}
		return *this;
	}

	Application& Application::operator=(Application &&other) noexcept
	{
		if(this != &other)
		{
			this->config = std::move(other.config);
			this->pWindow = std::exchange(other.pWindow, nullptr);
			this->loaded = std::move(other.loaded);
            this->newFrame = std::move(other.newFrame);
		}
		return *this;
	}

    void Application::Run()
    {
        if(pWindow)
        {
            std::cerr << "Window is already created\n";
            return;
        }

        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW\n";
            return;
        }

        //4.2 could work with compat profile
        // glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        // glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        // glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        if(config.flags & WindowFlags_Maximize)
            glfwWindowHint(GLFW_MAXIMIZED, GL_TRUE);

        GLFWmonitor* monitor =  glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

        if(config.flags & WindowFlags_Fullscreen)
        {
            pWindow = glfwCreateWindow(mode->width, mode->height, config.title.c_str(), monitor, nullptr);
        }   
        else
        {
            pWindow = glfwCreateWindow(config.width, config.height, config.title.c_str(), NULL, NULL);
        }
        
        if (!pWindow)
        {
            std::cerr << "Failed to create GLFW window" <<  '\n';
            Dispose();
            return;
        }

        glfwSetWindowUserPointer(pWindow, this);
        
        glfwSetFramebufferSizeCallback(pWindow, OnWindowResize);
        glfwSetWindowPosCallback(pWindow, OnWindowMove);
        glfwSetKeyCallback(pWindow, OnKeyPress);
        glfwSetMouseButtonCallback(pWindow, OnMouseButtonPress);
        glfwSetScrollCallback(pWindow, OnMouseScroll); 

        glfwMakeContextCurrent(pWindow);

        if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        {
            std::cerr << "Failed to initialize OpenGL\n";
            Dispose();
            return;
        }

        const GLubyte* version = glGetString(GL_VERSION);

        if (version != nullptr) 
            std::cout << "OpenGL Version: " << version << '\n';

        if(config.inconData.size() > 0)
        {
            Image image(config.inconData.data(), config.inconData.size());

            if(image.IsLoaded())
            {
                GLFWimage windowIcon;
                windowIcon.width = image.GetWidth();
                windowIcon.height = image.GetHeight();
                windowIcon.pixels = image.GetData();
                glfwSetWindowIcon(pWindow, 1, &windowIcon);
            }
        }

        if(config.flags & WindowFlags_VSync)
            glfwSwapInterval(1);
        else
            glfwSwapInterval(0);

        Initialize();

        if(loaded)
            loaded();

        glfwShowWindow(pWindow);

        while(!glfwWindowShouldClose(pWindow))
        {
			NewFrame();
            EndFrame();
            glfwSwapBuffers(pWindow);
            glfwPollEvents();
        }

        Deinitialize();

        Dispose();
    }

    void Application::Close()
    {
        if(pWindow)
        {
            glfwSetWindowShouldClose(pWindow, GLFW_TRUE);
        }
    }

    void Application::Quit()
    {
        if(instance)
        {
            instance->Close();
        }
    }

    void Application::Dispose()
    {
        if(!pWindow)
        {
            glfwTerminate();
        }
        else
        {
            glfwDestroyWindow(pWindow);
            glfwTerminate();
        }

        pWindow = nullptr;
    }

    GLFWwindow *Application::GetNativeWindow()
    {
        if(!instance)
            return nullptr;
        return instance->pWindow;
    }

    void Application::Initialize()
    {
        GLFWmonitor* monitor =  glfwGetPrimaryMonitor();
        const GLFWvidmode* mode = glfwGetVideoMode(monitor);

		Graphics::Initialize(config.width, config.height, mode->width, mode->height);
        Audio::Initialize(44100, 2);
        Input::Initialize();
        Physics::Initialize();
    }

	void Application::Deinitialize()
	{
        GameBehaviour::OnBehaviourApplicationQuit();
		Graphics::Deinitialize();
        Physics::Initialize();
        Audio::Deinitialize();
	}

	void Application::NewFrame()
	{
        Time::NewFrame();
        Input::NewFrame();
        Resources::NewFrame();
        GameBehaviour::NewFrame();
        Audio::NewFrame();
        Graphics::NewFrame();
	}

	void Application::EndFrame()
	{
        GameBehaviour::EndFrame();
        Input::EndFrame();
	}

    void Application::OnError(int32_t error_code, const char *description)
    {
        std::cerr << "GLFW error code " << error_code << ":" << description << '\n';
    }

    void Application::OnWindowResize(GLFWwindow *window, int32_t width, int32_t height)
    {
        Graphics::SetViewport(0, 0, width, height);
    }

    void Application::OnWindowMove(GLFWwindow *window, int32_t x, int32_t y)
    {
        Input::SetWindowPosition(x, y);
    }

    void Application::OnKeyPress(GLFWwindow *window, int32_t key, int32_t scancode, int32_t action, int32_t mods)
    {
        Input::SetKeyState((KeyCode)key, action > 0 ? 1 : 0);
    }

    void Application::OnCharPress(GLFWwindow *window, uint32_t codepoint)
    {
        Input::AddInputCharacter(codepoint);
    }

    void Application::OnMouseButtonPress(GLFWwindow *window, int32_t button, int32_t action, int32_t mods)
    {
        Input::SetButtonState((ButtonCode)button, action > 0 ? 1 : 0);
    }

    void Application::OnMouseScroll(GLFWwindow *window, double xoffset, double yoffset)
    {
        Input::SetScrollDirection(xoffset, yoffset);
    }
}