#include "Window.hpp"

#include <stb_image.h>

Window::Window(std::string title, int width, int height, bool vSync) : title(std::move(title)), width(width), height(height), vSync(vSync) {

    glfwInit();
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // the window will stay hidden after creation
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // the window will be resizable
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE); // make window maximized
    glfwWindowHint(GLFW_SAMPLES, 4);  // Request 4x MSAA for Anti-Aliasing

    this->glfwWindow = glfwCreateWindow(this->width, this->height, this->title.c_str(), nullptr, nullptr);

    if (glfwWindow == nullptr) {
        AT_FATAL("Error creating glfw window");
    }

    setWindowIcon(this->glfwWindow, "assets/textures/icon.png");

    glfwMakeContextCurrent(this->glfwWindow);

    glfwSetWindowUserPointer(this->glfwWindow, this);

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
        //Window &data = *static_cast<Window *>(glfwGetWindowUserPointer(window));

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

        if (action == GLFW_PRESS) {
            Keyboard::keyPressed[key] = true;
        } else if (action == GLFW_RELEASE) {
            Keyboard::keyPressed[key] = false;
        }
    });

    glfwSetCharCallback(glfwWindow, [](GLFWwindow *window, unsigned int codepoint) {
        //Keyboard::keyTyped.emplace(codepoint);
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

void Window::onUpdate() const {
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
