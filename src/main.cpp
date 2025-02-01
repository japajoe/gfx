#include "external/glad/glad.h"
#include "external/glfw/glfw3.h"
#include "external/imgui/ImGuiManager.hpp"
#include "external/imgui/imgui.h"
#include <iostream>

int main(int argc, char **argv) {
    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW!" << std::endl;
        return -1;
    }

    // Set GLFW to use OpenGL 4.6 (or the version you need)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(800, 600, "GLFW + Glad Example", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window!" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync



    // Initialize Glad to load OpenGL functions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize Glad!" << std::endl;
        return -1;
    }

    // Get OpenGL version info
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

	ImGuiManager imgui(window);

	imgui.Initialize();

    // Main rendering loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.0f, 0.2f, 0.3f, 1.0f); // Clear the background with a color
        glClear(GL_COLOR_BUFFER_BIT); // Clear the color buffer

		imgui.BeginFrame();
		ImGui::Begin("Hello");

		ImGui::End();
		imgui.EndFrame();

        // Swap buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

	imgui.Deinitialize();

    // Clean up and exit
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
