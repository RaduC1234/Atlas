#include "Event.hpp"
#include "Atlas.hpp"

class MouseEvent : public Event {
public:
    virtual ~MouseEvent() = default;

    // Common mouse event properties could go here
    float getX() const { return m_X; }
    float getY() const { return m_Y; }

protected:
    MouseEvent(float x = 0.0f, float y = 0.0f)
        : m_X(x), m_Y(y) {}

    float m_X;
    float m_Y;
};

class MouseButtonEvent : public MouseEvent {
public:
    MouseCode getButton() const { return m_Button; }

protected:
    MouseButtonEvent(MouseCode button, float x = 0.0f, float y = 0.0f)
        : MouseEvent(x, y), m_Button(button) {}

    MouseCode m_Button;
};

class MouseButtonPressedEvent : public MouseButtonEvent {
public:
    MouseButtonPressedEvent(MouseCode button, float x = 0.0f, float y = 0.0f)
        : MouseButtonEvent(button, x, y) {}

    EventType getType() const override { return EventType::MOUSE_BUTTON_PRESSED; }
    const char* getName() const override { return "MouseButtonPressed"; }

    std::string toString() const override {
        return "MouseButtonPressedEvent: " + std::to_string(static_cast<int>(getButton()));
    }

    static EventType getStaticType() { return EventType::MOUSE_BUTTON_PRESSED; }
};

class MouseButtonReleasedEvent : public MouseButtonEvent {
public:
    MouseButtonReleasedEvent(MouseCode button, float x = 0.0f, float y = 0.0f)
        : MouseButtonEvent(button, x, y) {}

    EventType getType() const override { return EventType::MOUSE_BUTTON_RELEASED; }
    const char* getName() const override { return "MouseButtonReleased"; }

    std::string toString() const override {
        return "MouseButtonReleasedEvent: " + std::to_string(static_cast<int>(getButton()));
    }

    static EventType getStaticType() { return EventType::MOUSE_BUTTON_RELEASED; }
};

class MouseMovedEvent : public MouseEvent {
public:
    MouseMovedEvent(float x, float y)
        : MouseEvent(x, y) {}

    EventType getType() const override { return EventType::MOUSE_MOVED; }
    const char* getName() const override { return "MouseMoved"; }

    std::string toString() const override {
        return "MouseMovedEvent: " + std::to_string(m_X) + ", " + std::to_string(m_Y);
    }

    static EventType getStaticType() { return EventType::MOUSE_MOVED; }
};

class MouseScrolledEvent : public MouseEvent {
public:
    MouseScrolledEvent(float xOffset, float yOffset)
        : m_XOffset(xOffset), m_YOffset(yOffset) {}

    float getXOffset() const { return m_XOffset; }
    float getYOffset() const { return m_YOffset; }

    EventType getType() const override { return EventType::MOUSE_SCROLLED; }
    const char* getName() const override { return "MouseScrolled"; }

    std::string toString() const override {
        return "MouseScrolledEvent: " + std::to_string(m_XOffset) + ", " + std::to_string(m_YOffset);
    }

    static EventType getStaticType() { return EventType::MOUSE_SCROLLED; }

private:
    float m_XOffset, m_YOffset;
};