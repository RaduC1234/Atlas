#pragma once
#include "Event.hpp"

class MouseEvent : public Event {
public:
    virtual ~MouseEvent() = default;
    virtual MouseCode getButton() const = 0;
};

class MouseButtonPressEvent : public MouseEvent {
public:
    explicit MouseButtonPressEvent(MouseCode button)
        : m_Button(button) {}

    MouseCode getButton() const override { return m_Button; }

    static EventType getStaticType() { return EventType::MOUSE_BUTTON_PRESSED; }
    EventType getType() const override { return getStaticType(); }

private:
    MouseCode m_Button;
};

class MouseButtonReleaseEvent : public MouseEvent {
public:
    explicit MouseButtonReleaseEvent(MouseCode button)
        : m_Button(button) {}

    MouseCode getButton() const override { return m_Button; }

    static EventType getStaticType() { return EventType::MOUSE_BUTTON_RELEASED; }
    EventType getType() const override { return getStaticType(); }

private:
    MouseCode m_Button;
};

class MouseMovedEvent : public MouseEvent {
public:
    MouseMovedEvent(int x, int y)
        : m_X(x), m_Y(y) {}

    MouseCode getButton() const override { return Mouse::ButtonLeft; }
    int getX() const { return m_X; }
    int getY() const { return m_Y; }

    static EventType getStaticType() { return EventType::MOUSE_MOVED; }
    EventType getType() const override { return getStaticType(); }

private:
    int m_X, m_Y;
};