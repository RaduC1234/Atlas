#include "Window.hpp"

#include <stb_image.h>
#include <chrono>
#include "Keyboard.hpp"
#include "Mouse.hpp"

Window::Window(std::string title, int width, int height, bool vSync)
    : title(std::move(title)), width(width), height(height), vSync(vSync), isDraggingWindow(false), currentStyle(Window::Style::DECORATED) {
    glfwInit();
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // the window will stay hidden after creation
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // the window will be resizable
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE); // make window maximized
    glfwWindowHint(GLFW_SAMPLES, 4); // Request 4x MSAA for Anti-Aliasing

    this->glfwWindow = glfwCreateWindow(this->width, this->height, this->title.c_str(), nullptr, nullptr);

    if (glfwWindow == nullptr) {
        AT_FATAL("Error creating glfw window");
    }

    setWindowIcon(this->glfwWindow, "assets/textures/iconAndLogo.png");

    glfwMakeContextCurrent(this->glfwWindow);
    glfwSetWindowUserPointer(this->glfwWindow, this);

    this->defaultCursor = loadCustomCursor("assets/textures/cursors/pointer_cursor.png", 0, 0);
    this->textCursor = loadCustomCursor("assets/textures/cursors/text_cursor.png", 16, 16);

    if (!defaultCursor || !textCursor) {
        AT_ERROR("Error loading cursors");
    }

    glfwSetWindowSizeCallback(glfwWindow, [](GLFWwindow* window, int width, int height) {
        Window& data = *static_cast<Window*>(glfwGetWindowUserPointer(window));
        data.width = width;
        data.height = height;
    });

    glfwSetWindowCloseCallback(glfwWindow, [](GLFWwindow* window) {
        Window& data = *static_cast<Window*>(glfwGetWindowUserPointer(window));
        if (data.closeCallback) {
            data.closeCallback();
        }
        glfwTerminate();
        exit(0);
    });

    glfwSetMouseButtonCallback(glfwWindow, [](GLFWwindow* window, int button, int action, int mods) {
        Window& data = *static_cast<Window*>(glfwGetWindowUserPointer(window));

        switch (action) {
            case GLFW_PRESS: {
                if (button < Mouse::buttonPressed.size()) {
                    Mouse::buttonPressed[button] = true;
                }

                // Start dragging only if left button is pressed and the window is undecorated
                if (button == GLFW_MOUSE_BUTTON_LEFT && data.currentStyle == Window::Style::UNDECORATED && Mouse::yPos < 30) {
                    data.isDraggingWindow = true;
                    glfwGetCursorPos(window, &data.dragStartMouseX, &data.dragStartMouseY);
                    glfwGetWindowPos(window, &data.windowStartX, &data.windowStartY);
                }
                break;
            }
            case GLFW_RELEASE: {
                if (button < Mouse::buttonPressed.size()) {
                    Mouse::buttonPressed[button] = false;
                }

                // Stop dragging when the left mouse button is released
                if (button == GLFW_MOUSE_BUTTON_LEFT) {
                    data.isDraggingWindow = false;
                }
                break;
            }
        }
    });


    glfwSetCursorPosCallback(glfwWindow, [](GLFWwindow* window, double xPos, double yPos) {
        Window& data = *static_cast<Window*>(glfwGetWindowUserPointer(window));

        Mouse::xPos = xPos;
        Mouse::yPos = yPos;

        if (data.isDraggingWindow && data.currentStyle == Window::Style::UNDECORATED) {
            static auto lastUpdateTime = std::chrono::steady_clock::now();
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - lastUpdateTime);

            if (elapsed.count() < 16) { // ~60 FPS
                return;
            }
            lastUpdateTime = now;

            double deltaX = xPos - data.dragStartMouseX;
            double deltaY = yPos - data.dragStartMouseY;

            int newPosX = static_cast<int>(data.windowStartX + deltaX);
            int newPosY = static_cast<int>(data.windowStartY + deltaY);

            glfwSetWindowPos(window, newPosX, newPosY);
        }

        Mouse::dragging = Mouse::buttonPressed[0] || Mouse::buttonPressed[1] || Mouse::buttonPressed[2];
    });

    glfwSwapInterval(1); // Enable VSync
    glfwShowWindow(glfwWindow);
    glfwMaximizeWindow(glfwWindow);
    glfwRestoreWindow(glfwWindow);
}

Window::~Window() {
    if (glfw_windowCount == 1)
        glfwTerminate();
    glfw_windowCount--;
}

void Window::onUpdate() const {
    if (Mouse::currentCursor == Mouse::Cursors::DEFAULT) {
        glfwSetCursor(this->glfwWindow, this->defaultCursor);
    } else if (Mouse::currentCursor == Mouse::Cursors::TEXT) {
        glfwSetCursor(this->glfwWindow, this->textCursor);
    }

    glfwPollEvents();
    glfwSwapBuffers(glfwWindow);
}

void Window::centerWindow() const {
    if (this->glfwWindow == nullptr) {
        AT_ERROR("GLFW window not initialized");
        return;
    }

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    if (!monitor || !mode) {
        AT_ERROR("Failed to get monitor information");
        return;
    }

    int xpos = (mode->width - this->width) / 2;
    int ypos = (mode->height - this->height) / 2;
    glfwSetWindowPos(this->glfwWindow, xpos, ypos);
}

void Window::setWindowSize(int newWidth, int newHeight) {
    if (this->glfwWindow == nullptr) {
        AT_ERROR("GLFW window not initialized");
        return;
    }

    this->width = newWidth;
    this->height = newHeight;
    glfwSetWindowSize(this->glfwWindow, newWidth, newHeight);
}

void Window::setWindowStyle(Style style) {
    if (this->glfwWindow == nullptr) {
        AT_ERROR("GLFW window not initialized");
        return;
    }

    switch (style) {
        case DECORATED:
            glfwSetWindowAttrib(this->glfwWindow, GLFW_DECORATED, GLFW_TRUE);
            currentStyle = Style::DECORATED;
            break;
        case UNDECORATED:
            glfwSetWindowAttrib(this->glfwWindow, GLFW_DECORATED, GLFW_FALSE);
            currentStyle = Style::UNDECORATED;
            break;
        default:
            AT_ERROR("Unknown window style");
            break;
    }
    glfwRestoreWindow(this->glfwWindow);
}

void Window::setWindowIcon(GLFWwindow* window, const char* iconPath) {
    GLFWimage images[1];
    images[0].pixels = stbi_load(iconPath, &images[0].width, &images[0].height, nullptr, 4); // RGBA
    if (images[0].pixels) {
        glfwSetWindowIcon(window, 1, images);
        stbi_image_free(images[0].pixels);
    } else {
        AT_ERROR("Error loading icon {0}", iconPath);
    }
}

GLFWcursor* Window::loadCustomCursor(const char* cursorImagePath, int hotspotX, int hotspotY) {
    int width, height, channels;
    unsigned char* imageData = stbi_load(cursorImagePath, &width, &height, &channels, 4); // Force 4 channels (RGBA)

    if (!imageData) {
        AT_ERROR("Failed to load cursor image: {0}", cursorImagePath);
        return nullptr;
    }

    GLFWimage glfwImage;
    glfwImage.width = width;
    glfwImage.height = height;
    glfwImage.pixels = imageData;

    GLFWcursor* cursor = glfwCreateCursor(&glfwImage, hotspotX, hotspotY);
    stbi_image_free(imageData);
    return cursor;
}
