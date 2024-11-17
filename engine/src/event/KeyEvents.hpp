#pragma once
#include "event/Event.hpp"
#include "Keyboard.hpp"

class KeyEvent : public Event {
public:
    KeyEvent(KeyCode keycode)
        : m_KeyCode(keycode) {}

    KeyCode getKeyCode() const { return m_KeyCode; }

    // Implement getName from base class
    virtual const char* getName() const override { return "KeyEvent"; }

protected:
    KeyCode m_KeyCode;
};

class KeyPressedEvent : public KeyEvent {
public:
    KeyPressedEvent(KeyCode keycode, bool isRepeat = false)
        : KeyEvent(keycode), m_IsRepeat(isRepeat) {}

    bool isRepeat() const { return m_IsRepeat; }

    static EventType getStaticType() { return EventType::KEY_PRESSED; }
    virtual EventType getType() const override { return getStaticType(); }
    virtual const char* getName() const override { return "KeyPressedEvent"; }

    virtual std::string toString() const override {
        std::stringstream ss;
        ss << "KeyPressedEvent: " << m_KeyCode << " (repeat = " << m_IsRepeat << ")";
        return ss.str();
    }

private:
    bool m_IsRepeat;
};

class KeyReleasedEvent : public KeyEvent {
public:
    KeyReleasedEvent(KeyCode keycode)
        : KeyEvent(keycode) {}

    static EventType getStaticType() { return EventType::KEY_RELEASED; }
    virtual EventType getType() const override { return getStaticType(); }
    virtual const char* getName() const override { return "KeyReleasedEvent"; }

    virtual std::string toString() const override {
        std::stringstream ss;
        ss << "KeyReleasedEvent: " << m_KeyCode;
        return ss.str();
    }
};