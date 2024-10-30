#pragma once
#include "event/Event.hpp"

class KeyEvent : public Event {
public:
    KeyEvent(Key::KeyCode keycode) : m_KeyCode(keycode) {}

    Key::KeyCode getKeyCode() const { return m_KeyCode; }

protected:
    Key::KeyCode m_KeyCode;
};

class KeyPressedEvent : public KeyEvent {
public:
    KeyPressedEvent(Key::KeyCode keycode, bool isRepeat = false)
        : KeyEvent(keycode), m_IsRepeat(isRepeat) {}

    bool isRepeat() const { return m_IsRepeat; }

    virtual EventType getType() const override { return EventType::KEY_PRESSED; }

private:
    bool m_IsRepeat;
};

class KeyReleasedEvent : public KeyEvent {
public:
    KeyReleasedEvent(Key::KeyCode keycode)
        : KeyEvent(keycode) {}

    virtual EventType getType() const override { return EventType::KEY_RELEASED; }
};