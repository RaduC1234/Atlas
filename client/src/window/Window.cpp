#include "Window.hpp"

#include <stb_image.h>

#include "Keyboard.hpp"
#include "Mouse.hpp"

Window::Window(std::string title, int width, int height, bool vSync) : title(std::move(title)), width(width), height(height), vSync(vSync) {
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

    setWindowIcon(this->glfwWindow, "assets/textures/icon.png");

    glfwMakeContextCurrent(this->glfwWindow);

    glfwSetWindowUserPointer(this->glfwWindow, this);

    this->defaultCursor = loadCustomCursor("assets/textures/cursors/pointer_cursor.png", 0, 0);
    this->textCursor = loadCustomCursor("assets/textures/cursors/text_cursor.png", 16, 16);
    //this->defaultCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    //this->textCursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);

    if (!defaultCursor || !textCursor) {
        AT_ERROR("Error loading cursors");
    }

    //https://www.glfw.org/docs/3.3/input_guide.html#input_key

    /**
    *  ===================================Window Callbacks=====================================
    */
    glfwSetWindowSizeCallback(glfwWindow, [](GLFWwindow *window, int width, int height) {
        Window &data = *static_cast<Window *>(glfwGetWindowUserPointer(window));

        data.width = width;
        data.height = height;
    });

    glfwSetWindowCloseCallback(glfwWindow, [](GLFWwindow *window) {
        Window &data = *static_cast<Window *>(glfwGetWindowUserPointer(window));

        if (data.closeCallback) {
            data.closeCallback();
        }

        glfwTerminate();
        exit(0);
    });


    /**
     *  ===================================Keyboard Callbacks=====================================
     */
    glfwSetKeyCallback(glfwWindow, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        if (key > Keyboard::keyPressed.size()) {
            AT_ERROR("Key out of bounds");
            return;
        }

        if (action == GLFW_PRESS || action == GLFW_REPEAT) {
            Keyboard::keyPressed[key] = true;
        } else if (action == GLFW_RELEASE) {
            Keyboard::keyPressed[key] = false;
        }
    });

    glfwSetCharCallback(glfwWindow, [](GLFWwindow *window, unsigned int codepoint) {
        if (!Keyboard::nativeInput) {
            return;
        }

        Keyboard::keyTyped.push(codepoint);
    });

    /**
     *  ===================================Mouse Callbacks=====================================
     */
    glfwSetMouseButtonCallback(glfwWindow, [](GLFWwindow *window, int button, int action, int mods) {
        switch (action) {
            case GLFW_PRESS: {
                if (button < Mouse::buttonPressed.size()) {
                    Mouse::buttonPressed[button] = true;
                }
                break;
            }
            case GLFW_RELEASE: {
                if (button < Mouse::buttonPressed.size()) {
                    Mouse::buttonPressed[button] = false;
                }
                break;
            }
        }
    });

    glfwSetScrollCallback(glfwWindow, [](GLFWwindow *window, double xOffset, double yOffset) {
        Mouse::scrollXOffset = xOffset;
        Mouse::scrollYOffset = yOffset;
    });

    glfwSetCursorPosCallback(glfwWindow, [](GLFWwindow *window, double xPos, double yPos) {
        Mouse::xPos = xPos;
        Mouse::yPos = yPos;
        Mouse::dragging = Mouse::buttonPressed[0] || Mouse::buttonPressed[1] || Mouse::buttonPressed[2];
    });

    // enable v-sync
    glfwSwapInterval(1);

    glfwShowWindow(glfwWindow);
    glfwMaximizeWindow(glfwWindow); // this fixes the wrong scaling issues in Camera
}

Window::~Window() {
    if (glfw_windowCount == 1)
        glfwTerminate();
    glfw_windowCount--;
}

// TODO: custom and modular way of loading and setting custom cursors
void Window::onUpdate() const {
    if (Mouse::currentCursor == Mouse::Cursors::DEFAULT) {
        glfwSetCursor(this->glfwWindow, this->defaultCursor);

    } else if (Mouse::currentCursor == Mouse::Cursors::TEXT) {
        glfwSetCursor(this->glfwWindow, this->textCursor);
    }

    glfwPollEvents();
    glfwSwapBuffers(glfwWindow);
}

void Window::setWindowIcon(GLFWwindow *window, const char *iconPath) {
    GLFWimage images[1];
    images[0].pixels = stbi_load(iconPath, &images[0].width, &images[0].height, nullptr, 4); //rgba channels
    if (images[0].pixels) {
        glfwSetWindowIcon(window, 1, images);
        stbi_image_free(images[0].pixels);
    } else {
        AT_ERROR("Error loading icon {0}", iconPath);
    }
}

GLFWcursor * Window::loadCustomCursor(const char *cursorImagePath, int hotspotX, int hotspotY) {
    int width, height, channels;
    unsigned char* imageData = stbi_load(cursorImagePath, &width, &height, &channels, 4); // Force 4 channels (RGBA)

    if (!imageData) {
        AT_ERROR("Failed to load cursor image: {0}", cursorImagePath)
        return nullptr;
    }

    GLFWimage glfwImage;
    glfwImage.width = width;
    glfwImage.height = height;
    glfwImage.pixels = imageData;


    GLFWcursor* cursor = glfwCreateCursor(&glfwImage, hotspotX, hotspotY);
    if (!cursor) {
        AT_ERROR("Failed to create custom cursor");
        stbi_image_free(imageData);
        return nullptr;
    }

    stbi_image_free(imageData);

    return cursor;
}

