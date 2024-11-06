#include "Window.hpp"


Window::Window(std::string title, int width, int height, bool vSync) : title(std::move(title)), width(width), height(height), vSync(vSync) {

    glfwInit();
    glfwDefaultWindowHints();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_VISIBLE, GLFW_FALSE); // the window will stay hidden after creation
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE); // the window will be resizable
    glfwWindowHint(GLFW_MAXIMIZED, GLFW_FALSE); // make window maximized
    glfwWindowHint(GLFW_SAMPLES, 4);  // Request 4x MSAA for Anti-Aliasing

    this->glfwWindow = glfwCreateWindow(this->width, this->height, this->title.c_str(), NULL, NULL);

    if (glfwWindow == nullptr) {
        AT_FATAL("Error creating glfw window");
    }

    setWindowIcon(this->glfwWindow, "resources/textures/default.png");

    glfwMakeContextCurrent(this->glfwWindow);

    glfwSetWindowUserPointer(this->glfwWindow, this);

    //https://www.glfw.org/docs/3.3/input_guide.html#input_key

    // Window Callbacks
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

    // Keyboard Callbacks
    glfwSetKeyCallback(glfwWindow, [](GLFWwindow *window, int key, int scancode, int action, int mods) {
        Window &data = *static_cast<Window *>(glfwGetWindowUserPointer(window));

        if (action == GLFW_PRESS) {
            //data.listener.keyPressed[key] = true;
        } else if (action == GLFW_RELEASE) {
            //data.listener.keyPressed[key] = false;
        }
    });
/*
            glfwSetCharCallback(glfwWindow, [](GLFWwindow *window, unsigned int keycode) {
                Window &data = *static_cast<Window *>(glfwGetWindowUserPointer(window));

                //KeyTypedEvent event(keycode);
                //data.getEventManager().publish(event);
            });*/

    // Mouse Callbacks
    glfwSetMouseButtonCallback(glfwWindow, [](GLFWwindow *window, int button, int action, int mods) {
        Window &data = *static_cast<Window *>(glfwGetWindowUserPointer(window));

        switch (action) {
            case GLFW_PRESS: {
                /* if (button < data.listener.mouseButtonPressed.size()) {
                     data.listener.mouseButtonPressed[button] = true;
                 }
                 break;*/
            }
            case GLFW_RELEASE: {
                /* if (button < data.listener.mouseButtonPressed.size()) {
                     data.listener.mouseButtonPressed[button] = false;
                 }
                 break;*/
            }
        }
    });

    glfwSetScrollCallback(glfwWindow, [](GLFWwindow *window, double xOffset, double yOffset) {
        Window &data = *static_cast<Window *>(glfwGetWindowUserPointer(window));

        /* data.listener.scrollX = xOffset;
         data.listener.scrollY = yOffset;*/
    });

    glfwSetCursorPosCallback(glfwWindow, [](GLFWwindow *window, double xPos, double yPos) {
        /*Window &data = *static_cast<Window *>(glfwGetWindowUserPointer(window));

        data.listener.lastX = data.listener.xPos;
        data.listener.lastY = data.listener.yPos;
        data.listener.xPos = xPos;
        data.listener.yPos = yPos;
        data.listener.dragging = data.listener.mouseButtonPressed[0] || data.listener.mouseButtonPressed[1] ||
                                 data.listener.mouseButtonPressed[2];*/
    });


/*        glfwSetFramebufferSizeCallback(glfwWindow, [](GLFWwindow *window, int l_width, int l_height) {
            glViewport(0, 0, l_width, l_height);
        });*/


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

void Window::onUpdate() {
    glfwPollEvents();
    glfwSwapBuffers(glfwWindow);
}

void Window::setWindowIcon(GLFWwindow *window, const char *iconPath) {
    GLFWimage images[1];
    images[0].pixels = stbi_load(iconPath, &images[0].width, &images[0].height, 0, 4); //rgba channels
    if (images[0].pixels) {
        glfwSetWindowIcon(window, 1, images);
        stbi_image_free(images[0].pixels);
    } else {
        AT_ERROR("Error loading icon {0}", iconPath);
    }
}
