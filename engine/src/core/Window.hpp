#pragma once

#include <utility>

#include "core/Core.hpp"
#include "event/Event.hpp"

#include "glm/glm.hpp"

struct WindowProps {
    std::string title;
    uint32_t width, height;
    bool vSync;

    WindowProps(std::string title = "Atlas Window", uint32_t width = 1080, uint32_t height = 720, bool vSync = true) : title(std::move(title)), width(width), height(height), vSync(vSync) {}
};

class Window {
public:

    using EventCallbackFn = std::function<void(Event &)>;

    virtual ~Window() = default;

    virtual void update() = 0;

    virtual uint32_t getWidth() const = 0;

    virtual uint32_t getHeight() const = 0;

    virtual void setEventCallback(const EventCallbackFn &callbackFn) = 0;

    virtual void setVSync(bool enabled) = 0;

    virtual bool isVSync() const = 0;

    virtual void *getNativeWindow() const = 0;

    static Scope<Window> create(const WindowProps &props = WindowProps());
};
