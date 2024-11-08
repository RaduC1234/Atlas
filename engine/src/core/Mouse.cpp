#include "Mouse.hpp"

std::vector<bool> Mouse::buttonPressed(8, false);
double Mouse::xPos = 0.0;
double Mouse::yPos = 0.0;
double Mouse::scrollXOffset = 0.0;
double Mouse::scrollYOffset = 0.0;
bool Mouse::dragging = false;

bool Mouse::isButtonPressed(MouseCode button) {
    if (button < buttonPressed.size()) {
        return buttonPressed[button];
    }
    return false;
}

std::pair<double, double> Mouse::getPosition() {
    return { xPos, yPos };
}

double Mouse::getX() {
    return xPos;
}

double Mouse::getY() {
    return yPos;
}

std::pair<double, double> Mouse::getScrollOffset() {
    return { scrollXOffset, scrollYOffset };
}

bool Mouse::isDragging() {
    return dragging;
}
