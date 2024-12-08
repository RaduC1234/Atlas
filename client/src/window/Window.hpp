#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <utility>


#include "core/Core.hpp"


class Window {
public:

    using CloseCallback = std::function<void()>;

    Window(std::string title = "Atlas Window", int width = 1920, int height = 1080, bool vSync = true);

    ~Window();

    void onUpdate() const;

    int getWidth() const {
        return width;
    }

    int getHeight() const {
        return height;
    }

    void setCloseCallback(const CloseCallback &callback) {
        this->closeCallback = callback;
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
    CloseCallback closeCallback = nullptr;

    GLFWwindow *glfwWindow = nullptr;

    GLFWcursor* defaultCursor;
    GLFWcursor* textCursor;
    uint8_t currentCursor;

    inline static int glfw_windowCount = 0;
};
