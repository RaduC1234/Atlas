#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Atlas.hpp>

class Window {
public:

    enum Style : uint32_t {
        DECORATED = BIT(0),
        UNDECORATED = BIT(1),
        MAXIMEZED = BIT(2),
        MINIMIZED = BIT(3)
    };

    using CloseCallback = std::function<void()>;

    Window(std::string title = "Atlas Window", int width = 1920, int height = 1080, bool vSync = true);

    ~Window();

    void onUpdate() const;

    void centerWindow() const;

    void setWindowSize(int newWidth, int newHeight);

    void setWindowStyle(uint32_t style);

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

    std::pair<int, int> getMonitorSize();

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

    bool isDragging{false};
    int initialCursorX, initialCursorY;
    int cursorOffsetX, cursorOffsetY;
    int initialWindowX, initialWindowY;
    bool decorated{true};

    inline static int glfw_windowCount = 0;
};
