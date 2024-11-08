#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>


#include "core/Core.hpp"

class Window {
public:

    Window(std::string title = "Atlas Window", int width = 1920, int height = 1080, bool vSync = true);

    ~Window();

    void onUpdate();

    int getWidth() const {
        return width;
    }

    int getHeight() const {
        return height;
    }

    GLFWwindow *getNativeWindow() const {
        return glfwWindow;
    }

    friend class Application;

private:

    static void setWindowIcon(GLFWwindow *window, const char *iconPath);

    std::string title = "Avalon C++";
    int width;
    int height;
    bool vSync;

    GLFWwindow *glfwWindow = nullptr;

    inline static int glfw_windowCount = 0;
};
