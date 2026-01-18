/**
 * File   : Input.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#include "engine/input/Input.hpp"

namespace aer::input
{
    Input &Input::instance()
    {
        static Input instance;
        return instance;
    }

    void Input::beginFrame()
    {
        _keysPressed.fill(false);
        _keysReleased.fill(false);
        _mousePressed.fill(false);
        _mouseReleased.fill(false);
        _wheelDelta = 0.0f;
        _closeRequested = false;
        _events.clear();
    }

    void Input::processEvents(const std::vector<Event> &events)
    {
        beginFrame();
        _events = events;

        for (const auto &event : events) {
            switch (event.type) {
                case EventType::Close:
                    _closeRequested = true;
                    break;
                case EventType::KeyDown: {
                    const auto idx = keyIndex(event.key);
                    if (event.key != KeyCode::Unknown && !_keysDown[idx]) {
                        _keysPressed[idx] = true;
                    }
                    if (event.key != KeyCode::Unknown) {
                        _keysDown[idx] = true;
                    }
                    break;
                }
                case EventType::KeyUp: {
                    const auto idx = keyIndex(event.key);
                    if (event.key != KeyCode::Unknown && _keysDown[idx]) {
                        _keysReleased[idx] = true;
                    }
                    if (event.key != KeyCode::Unknown) {
                        _keysDown[idx] = false;
                    }
                    break;
                }
                case EventType::MouseMove:
                    _mouseX = event.x;
                    _mouseY = event.y;
                    break;
                case EventType::MouseButtonDown: {
                    const auto idx = mouseIndex(event.mouseButton);
                    if (event.mouseButton != MouseButton::Unknown &&
                        !_mouseDown[idx]) {
                        _mousePressed[idx] = true;
                    }
                    if (event.mouseButton != MouseButton::Unknown) {
                        _mouseDown[idx] = true;
                    }
                    _mouseX = event.x;
                    _mouseY = event.y;
                    break;
                }
                case EventType::MouseButtonUp: {
                    const auto idx = mouseIndex(event.mouseButton);
                    if (event.mouseButton != MouseButton::Unknown &&
                        _mouseDown[idx]) {
                        _mouseReleased[idx] = true;
                    }
                    if (event.mouseButton != MouseButton::Unknown) {
                        _mouseDown[idx] = false;
                    }
                    _mouseX = event.x;
                    _mouseY = event.y;
                    break;
                }
                case EventType::MouseWheel:
                    _wheelDelta += event.wheelDelta;
                    _mouseX = event.x;
                    _mouseY = event.y;
                    break;
                case EventType::None:
                    break;
                default:
                    break;
            }
        }
    }

    bool Input::isKeyDown(KeyCode key) const
    {
        return (key != KeyCode::Unknown) && _keysDown[keyIndex(key)];
    }

    bool Input::isKeyPressed(KeyCode key) const
    {
        return (key != KeyCode::Unknown) && _keysPressed[keyIndex(key)];
    }

    bool Input::isKeyReleased(KeyCode key) const
    {
        return (key != KeyCode::Unknown) && _keysReleased[keyIndex(key)];
    }

    bool Input::isMouseDown(MouseButton button) const
    {
        return (button != MouseButton::Unknown) && _mouseDown[mouseIndex(button)];
    }

    bool Input::isMousePressed(MouseButton button) const
    {
        return (button != MouseButton::Unknown) && _mousePressed[mouseIndex(button)];
    }

    bool Input::isMouseReleased(MouseButton button) const
    {
        return (button != MouseButton::Unknown) && _mouseReleased[mouseIndex(button)];
    }

    int Input::mouseX() const
    {
        return _mouseX;
    }

    int Input::mouseY() const
    {
        return _mouseY;
    }

    float Input::wheelDelta() const
    {
        return _wheelDelta;
    }

    bool Input::closeRequested() const
    {
        return _closeRequested;
    }

    const std::vector<Event> &Input::events() const
    {
        return _events;
    }

    std::size_t Input::keyIndex(KeyCode key)
    {
        return static_cast<std::size_t>(key);
    }

    std::size_t Input::mouseIndex(MouseButton button)
    {
        return static_cast<std::size_t>(button);
    }
}
