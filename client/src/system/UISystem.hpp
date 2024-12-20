#pragma once

#include <Atlas.hpp>
#include "window/Keyboard.hpp"
#include "window/Mouse.hpp"

class UISystem {
public:
    void update(double deltaTime, Registry &registry, const Camera &camera) {
        auto [mouseX, mouseY] = Mouse::getPosition();
        const auto worldCoords = camera.screenToWorld(glm::vec2(mouseX, mouseY));
        //textBox functions
        auto textBoxView = registry.view<TransformComponent, TextboxComponent>();
        bool anyTextboxClicked = false;

        for (auto entity: textBoxView) {
            auto &transform = textBoxView.get<TransformComponent>(entity);
            auto &textbox = textBoxView.get<TextboxComponent>(entity);

            if (isMouseOver(worldCoords, transform)) {
                Mouse::setCursor(Mouse::Cursors::TEXT);

                if (Mouse::isButtonPressed(Mouse::ButtonLeft)) {
                    focusedEntity = entity;
                    anyTextboxClicked = true;
                    textbox.isFocused = true;
                    Keyboard::nativeInput = true;
                }
            }

            if (focusedEntity == entity) {
                if (!anyTextboxClicked && Mouse::isButtonPressed(Mouse::ButtonLeft)) {
                    // Defocus if clicked outside
                    textbox.isFocused = false;
                    focusedEntity = entt::null;
                    Keyboard::nativeInput = false;
                } else {
                    updateCursorBlink(deltaTime, textbox);
                }
            } else {
                removeCursor(textbox);
            }
        }

        if (focusedEntity == entt::null) {
            Mouse::setCursor(Mouse::Cursors::DEFAULT);
        }

        if (focusedEntity != entt::null) {
            auto &textbox = registry.get<TextboxComponent>(focusedEntity);
            handleTyping(deltaTime, textbox);
        }


        //button functions
        auto buttonView = registry.view<TransformComponent, RenderComponent, ButtonComponent>();

        for (auto entity : buttonView) {
            auto& transform = buttonView.get<TransformComponent>(entity);
            auto& render = buttonView.get<RenderComponent>(entity);
            auto& button = buttonView.get<ButtonComponent>(entity);

            if (isMouseOver(worldCoords, transform)) {
                Mouse::setCursor(Mouse::Cursors::DEFAULT);

                if (!button.isHovered) {
                    button.isHovered = true;
                    updateButtonVisuals(render, button);
                    if (button.onHover) {
                        button.onHover();
                    }
                }

                if (Mouse::isButtonPressed(Mouse::ButtonLeft)) {
                    if (!button.isPressed) {
                        button.isPressed = true;
                        updateButtonVisuals(render, button);
                        if (button.onPressed) {
                            button.onPressed();
                        }
                    }
                } else if (button.isPressed) {
                    button.isPressed = false;
                    if (button.onClick) {
                        button.onClick();
                    }
                    updateButtonVisuals(render, button);
                }
            } else if (button.isHovered) {
                button.isHovered = false;
                button.isPressed = false;
                updateButtonVisuals(render, button);
            }
        }
    }

private:
    float cursorBlinkTime = 0.5f;
    bool cursorVisible = true;
    double timeSinceLastBlink = 0.0;

    entt::entity focusedEntity = entt::null;

    float keyRepeatDelay = 0.2f;
    double timeSinceLastKeyAction = 0.0;

    constexpr bool isMouseOver(glm::vec2 mouse, const TransformComponent &transform) {
        glm::vec2 halfSize = transform.scale * 0.5f;
        return mouse.x >= (transform.position.x - halfSize.x) &&
               mouse.x <= (transform.position.x + halfSize.x) &&
               mouse.y >= (transform.position.y - halfSize.y) &&
               mouse.y <= (transform.position.y + halfSize.y);
    }

    void handleTyping(double deltaTime, TextboxComponent &textbox) {
        removeCursor(textbox);

        timeSinceLastKeyAction += deltaTime;

        if (Keyboard::isKeyPressed(Keyboard::Backspace)) {
            if (timeSinceLastKeyAction >= keyRepeatDelay) {
                if (textbox.cursorPosition > 0) {
                    textbox.text.erase(textbox.cursorPosition - 1, 1);
                    textbox.cursorPosition--;
                }
                timeSinceLastKeyAction = 0.0;
            }
        }

        if (Keyboard::isKeyPressed(Keyboard::Left)) {
            if (timeSinceLastKeyAction >= keyRepeatDelay) {
                if (textbox.cursorPosition > 0) {
                    textbox.cursorPosition--;
                }
                timeSinceLastKeyAction = 0.0;
            }
        } else if (Keyboard::isKeyPressed(Keyboard::Right)) {
            if (timeSinceLastKeyAction >= keyRepeatDelay) {
                if (textbox.cursorPosition < textbox.text.size()) {
                    textbox.cursorPosition++;
                }
                timeSinceLastKeyAction = 0.0;
            }
        }

        while (!Keyboard::keyTyped.empty()) {
            uint32_t typedChar = Keyboard::keyTyped.front();
            Keyboard::keyTyped.pop();

            if (typedChar > 127) {
                continue;
            }

            if (textbox.canAddCharacter()) {
                textbox.text.insert(textbox.cursorPosition, 1, static_cast<char>(typedChar));
                textbox.cursorPosition++;
            }
        }

        addCursor(textbox);
    }

    constexpr void addCursor(TextboxComponent &textbox) {
        if (textbox.isFocused && cursorVisible) {
            removeCursor(textbox);
            if (textbox.cursorPosition <= textbox.text.size()) {
                textbox.text.insert(textbox.cursorPosition, 1, '|');
            }
        }
    }

    constexpr void removeCursor(TextboxComponent &textbox) {
        size_t cursorIndex = textbox.text.find('|');
        if (cursorIndex != std::string::npos) {
            textbox.text.erase(cursorIndex, 1);
        }
    }

    constexpr void updateCursorBlink(double deltaTime, TextboxComponent &textbox) {
        timeSinceLastBlink += deltaTime;

        if (timeSinceLastBlink >= cursorBlinkTime) {
            cursorVisible = !cursorVisible;
            timeSinceLastBlink = 0.0;

            if (cursorVisible) {
                addCursor(textbox);
            } else {
                removeCursor(textbox);
            }
        }
    }

    void updateButtonVisuals(RenderComponent& render, const ButtonComponent& button) {
        if (button.isPressed) {
            render.color = button.pressedColor;
        } else if (button.isHovered) {
            render.color = button.hoverColor;
        } else {
            render.color = button.normalColor;
        }
    }
};