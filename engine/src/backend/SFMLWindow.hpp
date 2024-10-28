#pragma once

#include "core/Window.hpp"

#include <SFML/Graphics.hpp>

class SFMLWindow : public Window {
public:
    SFMLWindow(const WindowProps& windowProps);

    ~SFMLWindow() override = default;

    void update() override;

    uint32_t getWidth() const override;

    uint32_t getHeight() const override;

    void setEventCallback(const EventCallbackFn &callbackFn) override;

    void setVSync(bool enabled) override;

    bool isVSync() const override;

    void *getNativeWindow() const override;

private:
    sf::RenderWindow m_window;
    WindowProps m_props;
    EventCallbackFn m_eventCallback;
    bool m_vsyncEnabled;
};
