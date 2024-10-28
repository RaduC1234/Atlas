#include "ClientApplication.hpp"
#include "core/PlatformUtils.hpp"


void ClientApplication::run() {

    Log::init();
    AT_INFO("Starting Atlas Client");

    this->window = Window::create();

    while (true) {

        float beginTime = Time::getTime();
        float endTime;
        float deltaTime = -1.0f;

        this->window->update();

        // rendering here


        endTime = Time::getTime();
        deltaTime = endTime - beginTime;
        beginTime = endTime;
    }
}
