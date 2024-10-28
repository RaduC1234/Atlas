#include "SFMLWindow.hpp"

SFMLWindow::SFMLWindow(const WindowProps &windowProps): m_props(windowProps), m_vsyncEnabled(windowProps.vSync) {

    sf::VideoMode videoMode(m_props.width, m_props.height);
    m_window.create(videoMode, m_props.title);
    m_window.setVerticalSyncEnabled(m_vsyncEnabled);
}

void SFMLWindow::update() {
    sf::Event event;
    while (m_window.pollEvent(event)) {
        if(event.type == sf::Event::Closed)
           m_window.close();
/*        if (m_eventCallback) { TODO: implement window events into internal event system
            m_eventCallback(event);
        }
        if (event.type == sf::Event::Closed) {
            m_window.close();
        }*/
    }
    this->m_window.display();
    this->m_window.clear();

}

uint32_t SFMLWindow::getWidth() const {
    return m_props.width;
}

uint32_t SFMLWindow::getHeight() const {
    return m_props.height;
}

void SFMLWindow::setEventCallback(const EventCallbackFn &callbackFn) {
    m_eventCallback = callbackFn;
}

void SFMLWindow::setVSync(bool enabled) {
    m_vsyncEnabled = enabled;
    m_window.setVerticalSyncEnabled(m_vsyncEnabled);
}

bool SFMLWindow::isVSync() const {
    return m_vsyncEnabled;
}

void* SFMLWindow::getNativeWindow() const {
    return m_window.getSystemHandle();
}
