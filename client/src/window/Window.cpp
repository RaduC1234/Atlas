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

    setWindowIcon(this->glfwWindow, "assets/textures/iconAndLogo.png");

    glfwMakeContextCurrent(this->glfwWindow);

    glfwSetWindowUserPointer(this->glfwWindow, this);

    //this->defaultCursor = loadCustomCursor("assets/textures/cursors/pointer_cursor.png", 0, 0);
    //this->textCursor = loadCustomCursor("assets/textures/cursors/text_cursor.png", 16, 16);
    this->defaultCursor = glfwCreateStandardCursor(GLFW_ARROW_CURSOR);
    this->textCursor = glfwCreateStandardCursor(GLFW_IBEAM_CURSOR);

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
        Window &data = *static_cast<Window *>(glfwGetWindowUserPointer(window));

        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                // Start dragging: Record initial cursor and window position
                data.isDragging = true;

                double cursor_x, cursor_y;
                glfwGetCursorPos(window, &cursor_x, &cursor_y);
                data.initialCursorX = static_cast<int>(cursor_x);
                data.initialCursorY = static_cast<int>(cursor_y);

                glfwGetWindowPos(window, &data.initialWindowX, &data.initialWindowY);
            } else if (action == GLFW_RELEASE) {
                // Stop dragging: Reset dragging state
                data.isDragging = false;
            }
        }

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

    glfwMaximizeWindow(glfwWindow); // this fixes the wrong scaling issues in Camera
    glfwRestoreWindow(glfwWindow);

    glfwShowWindow(glfwWindow);
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

    if (isDragging) {
        double cursor_x, cursor_y;
        glfwGetCursorPos(this->glfwWindow, &cursor_x, &cursor_y);

        // Calculate offsets relative to the initial positions
        int delta_x = static_cast<int>(cursor_x) - initialCursorX;
        int delta_y = static_cast<int>(cursor_y) - initialCursorY;

        // Move the window to the new position
        glfwSetWindowPos(this->glfwWindow, initialWindowX + delta_x, initialWindowY + delta_y);
    }
}

void Window::centerWindow() const {
    if (this->glfwWindow == nullptr) {
        AT_ERROR("GLFW window not initialized");
        return;
    }

    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    if (!monitor) {
        AT_ERROR("Failed to get primary monitor");
        return;
    }

    const GLFWvidmode* mode = glfwGetVideoMode(monitor);
    if (!mode) {
        AT_ERROR("Failed to get video mode of primary monitor");
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

std::pair<int, int> Window::getMonitorSize() {
    if (!glfwInit()) {
        throw std::runtime_error("Failed to initialize GLFW");
    }

    GLFWmonitor* monitor = glfwGetWindowMonitor(this->glfwWindow);
    if (!monitor) {
        int windowX, windowY, windowWidth, windowHeight;
        glfwGetWindowPos(this->glfwWindow, &windowX, &windowY);
        glfwGetWindowSize(this->glfwWindow, &windowWidth, &windowHeight);

        int monitorCount;
        GLFWmonitor** monitors = glfwGetMonitors(&monitorCount);
        if (!monitors) {
            throw std::runtime_error("Failed to get monitors");
        }

        for (int i = 0; i < monitorCount; ++i) {
            int monitorX, monitorY, monitorWidth, monitorHeight;
            glfwGetMonitorWorkarea(monitors[i], &monitorX, &monitorY, &monitorWidth, &monitorHeight);

            if (windowX >= monitorX && windowX < monitorX + monitorWidth &&
                windowY >= monitorY && windowY < monitorY + monitorHeight) {
                monitor = monitors[i];
                break;
                }
        }

        if (!monitor) {
            throw std::runtime_error("Could not determine the monitor for the window");
        }
    }

    int x, y, width, height;
    glfwGetMonitorWorkarea(monitor, &x, &y, &width, &height);

    return {width, height};
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

void Window::setWindowStyle(Style style) {
    if (this->glfwWindow == nullptr) {
        AT_ERROR("GLFW window not initialized");
        return;
    }

    switch (style) {
        case DECORATED:
            glfwSetWindowAttrib(this->glfwWindow, GLFW_DECORATED, GLFW_TRUE);
        break;
        case UNDECORATED:
            glfwSetWindowAttrib(this->glfwWindow, GLFW_DECORATED, GLFW_FALSE);
        break;
        default:
            AT_ERROR("Unknown window style");
        break;
    }
    glfwRestoreWindow(this->glfwWindow);
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

