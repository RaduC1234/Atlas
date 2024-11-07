#include "Keyboard.hpp"

std::vector<bool> Keyboard::keyPressed(349, false);

bool Keyboard::isKeyPressed(KeyCode code) {
    if (code < keyPressed.size()) {
        return keyPressed[code];
    }
    return false;
}

