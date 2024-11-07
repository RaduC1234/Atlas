#pragma once
#include "event/Event.hpp"

using MouseCode = uint16_t;

class Mouse
{
public:
    /**
    * @brief Checks if a mouse button is currently pressed.
    * @param button The MouseCode representing the mouse button.
    * @return true if the button is pressed, false otherwise.
    */
    static bool isButtonPressed(MouseCode button);

    /**
     * @brief Gets the current position of the mouse cursor.
     * @return A pair (x, y) representing the current cursor position.
     */
    static std::pair<double, double> getPosition();

    /**
     * @brief Gets the x-coordinate of the mouse cursor.
     * @return The x-coordinate of the mouse cursor.
     */
    static double getX();

    /**
     * @brief Gets the y-coordinate of the mouse cursor.
     * @return The y-coordinate of the mouse cursor.
     */
    static double getY();

    /**
     * @brief Gets the current scroll offset.
     * @return A pair (xOffset, yOffset) representing the scroll offset.
     */
    static std::pair<double, double> getScrollOffset();


    /*
     * @brief Helper function for mouse dragging.
     * @return True if any of the mouse buttons is pressed, false otherwise
     */
    static bool isDragging();

    enum : MouseCode
    {
        // From glfw3.h
        Button0                = 0,
        Button1                = 1,
        Button2                = 2,
        Button3                = 3,
        Button4                = 4,
        Button5                = 5,
        Button6                = 6,
        Button7                = 7,

        ButtonLast             = Button7,
        ButtonLeft             = Button0,
        ButtonRight            = Button1,
        ButtonMiddle           = Button2
    };

private:
    static std::vector<bool> buttonPressed;
    static double xPos, yPos;
    static double scrollXOffset, scrollYOffset;
    static bool dragging;

    friend class Window;
};
