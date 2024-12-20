#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Atlas.hpp>


class Window {
public:

    enum Style {
        DECORATED,
        UNDECORATED
    };

    using CloseCallback = std::function<void()>;

    Window(std::string title = "Atlas Window", int width = 1920, int height = 1080, bool vSync = true);

    ~Window();

    void onUpdate() const;

    void centerWindow() const;

    void setWindowSize(int newWidth, int newHeight);

    void setWindowStyle(Style style);

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
    static GLFWcursor* loadCustomCursor(const char* cursorImagePath, int hotspotX, int hotspotY);

    std::string title = "Avalon C++";
    int width;
    int height;
    bool vSync;
    CloseCallback closeCallback{nullptr};

    GLFWwindow *glfwWindow{nullptr};

    GLFWcursor* defaultCursor{nullptr};
    GLFWcursor* textCursor{nullptr};
    uint8_t currentCursor{0};

    bool isDraggingWindow{false};
    double dragStartMouseX = 0.0, dragStartMouseY = 0.0;
    int windowStartX = 0, windowStartY = 0;
    Style currentStyle{DECORATED};


    inline static int glfw_windowCount = 0;
};
