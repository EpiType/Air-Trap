/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** UIRenderSystem.cpp
*/

#include "Systems/UIRenderSystem.hpp"
#include "RType/ECS/ZipView.hpp"
#include "RType/Logger.hpp"

namespace Client::Systems {

UIRenderSystem::UIRenderSystem(rtp::ecs::Registry& registry, 
                               sf::RenderWindow& window)
    : _registry(registry), _window(window) {}

void UIRenderSystem::update(float dt) {
    (void)dt;
    renderButtons();
    renderTexts();
    renderSliders();
    renderDropdowns();
    renderTextInputs(dt);
}

void UIRenderSystem::renderButtons() {
    auto buttonsResult = _registry.get<rtp::ecs::components::ui::Button>();
    if (!buttonsResult) return;
    
    auto& buttons = buttonsResult.value().get();
    for (const auto& entity : buttons.entities()) {
        auto& button = buttons[entity];
        
        sf::RectangleShape rect(sf::Vector2f(button.size.x, button.size.y));
        rect.setPosition(sf::Vector2f(button.position.x, button.position.y));
        
        const uint8_t* color = button.idleColor;
        if (button.state == rtp::ecs::components::ui::ButtonState::Hovered) {
            color = button.hoverColor;
        } else if (button.state == rtp::ecs::components::ui::ButtonState::Pressed) {
            color = button.pressedColor;
        }
        
        rect.setFillColor(sf::Color(color[0], color[1], color[2]));
        rect.setOutlineColor(sf::Color::White);
        rect.setOutlineThickness(2.0f);
        
        _window.draw(rect);
        
        try {
            sf::Font& font = loadFont("assets/fonts/main.ttf");
            sf::Text text(font);
            text.setString(sf::String::fromUtf8(button.text.begin(), button.text.end()));
            text.setCharacterSize(24);
            
            sf::FloatRect textBounds = text.getLocalBounds();
            text.setOrigin(sf::Vector2f(textBounds.size.x / 2.0f, textBounds.size.y / 2.0f));
            text.setPosition(sf::Vector2f(
                button.position.x + button.size.x / 2.0f,
                button.position.y + button.size.y / 2.0f - 5.0f
            ));
            text.setFillColor(sf::Color::White);
            
            _window.draw(text);
        } catch (const std::exception& e) {
            rtp::log::error("Failed to render button text: {}", e.what());
        }
    }
}

void UIRenderSystem::renderTexts() {
    auto textsResult = _registry.get<rtp::ecs::components::ui::Text>();
    if (!textsResult) return;
    
    auto& texts = textsResult.value().get();
    for (const auto& entity : texts.entities()) {
        auto& textComp = texts[entity];
        
        try {
            sf::Font& font = loadFont(textComp.fontPath);
            sf::Text text(font);
            text.setString(sf::String::fromUtf8(textComp.content.begin(), textComp.content.end()));
            text.setCharacterSize(textComp.fontSize);
            text.setPosition(sf::Vector2f(textComp.position.x, textComp.position.y));
            text.setFillColor(sf::Color(
                textComp.red, textComp.green, textComp.blue, textComp.alpha));
            
            _window.draw(text);
        } catch (const std::exception& e) {
            rtp::log::error("Failed to render text '{}': {}", 
                          textComp.content, e.what());
        }
    }
}

sf::Font& UIRenderSystem::loadFont(const std::string& fontPath) {
    auto it = _fonts.find(fontPath);
    if (it != _fonts.end()) {
        return it->second;
    }
    
    sf::Font font;
    if (!font.openFromFile(fontPath)) {
        throw std::runtime_error("Failed to load font: " + fontPath);
    }
    
    _fonts[fontPath] = std::move(font);
    return _fonts[fontPath];
}

void UIRenderSystem::renderSliders() {
    auto slidersResult = _registry.get<rtp::ecs::components::ui::Slider>();
    if (!slidersResult) return;
    
    auto& sliders = slidersResult.value().get();
    for (const auto& entity : sliders.entities()) {
        auto& slider = sliders[entity];
        
        sf::RectangleShape track(sf::Vector2f(slider.size.x, slider.size.y));
        track.setPosition(sf::Vector2f(slider.position.x, slider.position.y));
        track.setFillColor(sf::Color(slider.trackColor[0], slider.trackColor[1], slider.trackColor[2]));
        _window.draw(track);
        
        float fillWidth = slider.size.x * slider.getNormalized();
        sf::RectangleShape fill(sf::Vector2f(fillWidth, slider.size.y));
        fill.setPosition(sf::Vector2f(slider.position.x, slider.position.y));
        fill.setFillColor(sf::Color(slider.fillColor[0], slider.fillColor[1], slider.fillColor[2]));
        _window.draw(fill);
        
        float handleX = slider.position.x + fillWidth - 5.0f;
        sf::RectangleShape handle(sf::Vector2f(10.0f, slider.size.y + 8.0f));
        handle.setPosition(sf::Vector2f(handleX, slider.position.y - 4.0f));
        handle.setFillColor(sf::Color(slider.handleColor[0], slider.handleColor[1], slider.handleColor[2]));
        _window.draw(handle);
    }
}

void UIRenderSystem::renderDropdowns() {
    auto dropdownsResult = _registry.get<rtp::ecs::components::ui::Dropdown>();
    if (!dropdownsResult) return;
    
    auto& dropdowns = dropdownsResult.value().get();
    
    for (const auto& entity : dropdowns.entities()) {
        auto& dropdown = dropdowns[entity];
        
        sf::RectangleShape button(sf::Vector2f(dropdown.size.x, dropdown.size.y));
        button.setPosition(sf::Vector2f(dropdown.position.x, dropdown.position.y));
        button.setFillColor(sf::Color(dropdown.bgColor[0], dropdown.bgColor[1], dropdown.bgColor[2]));
        button.setOutlineColor(sf::Color::White);
        button.setOutlineThickness(2.0f);
        _window.draw(button);
        
        try {
            sf::Font& font = loadFont("assets/fonts/main.ttf");
            std::string selectedText = dropdown.getSelected();
            sf::Text text(font);
            text.setString(sf::String::fromUtf8(selectedText.begin(), selectedText.end()));
            text.setCharacterSize(20);
            text.setPosition(sf::Vector2f(dropdown.position.x + 10.0f, dropdown.position.y + 8.0f));
            text.setFillColor(sf::Color(dropdown.textColor[0], dropdown.textColor[1], dropdown.textColor[2]));
            _window.draw(text);
            
            sf::Text arrow(font);
            arrow.setString(dropdown.isOpen ? "^" : "v");
            arrow.setCharacterSize(16);
            arrow.setPosition(sf::Vector2f(dropdown.position.x + dropdown.size.x - 30.0f, dropdown.position.y + 10.0f));
            arrow.setFillColor(sf::Color::White);
            _window.draw(arrow);
        } catch (const std::exception& e) {
            rtp::log::error("Failed to render dropdown: {}", e.what());
        }
    }
    
    for (const auto& entity : dropdowns.entities()) {
        auto& dropdown = dropdowns[entity];
        
        if (dropdown.isOpen) {
            try {
                sf::Font& font = loadFont("assets/fonts/main.ttf");
                float optionY = dropdown.position.y + dropdown.size.y;
                
                for (size_t i = 0; i < dropdown.options.size(); ++i) {
                    sf::RectangleShape optionBg(sf::Vector2f(dropdown.size.x, dropdown.size.y));
                    optionBg.setPosition(sf::Vector2f(dropdown.position.x, optionY));
                    
                    if (static_cast<int>(i) == dropdown.hoveredIndex) {
                        optionBg.setFillColor(sf::Color(dropdown.hoverColor[0], dropdown.hoverColor[1], dropdown.hoverColor[2]));
                    } else {
                        optionBg.setFillColor(sf::Color(dropdown.bgColor[0], dropdown.bgColor[1], dropdown.bgColor[2]));
                    }
                    optionBg.setOutlineColor(sf::Color::White);
                    optionBg.setOutlineThickness(1.0f);
                    _window.draw(optionBg);
                    
                    sf::Text optionText(font);
                    optionText.setString(sf::String::fromUtf8(dropdown.options[i].begin(), dropdown.options[i].end()));
                    optionText.setCharacterSize(20);
                    optionText.setPosition(sf::Vector2f(dropdown.position.x + 10.0f, optionY + 8.0f));
                    optionText.setFillColor(sf::Color::White);
                    _window.draw(optionText);
                    
                    optionY += dropdown.size.y;
                }
            } catch (const std::exception& e) {
                rtp::log::error("Failed to render dropdown options: {}", e.what());
            }
        }
    }
}

void UIRenderSystem::renderTextInputs(float dt)
{
    auto inputsResult = _registry.get<rtp::ecs::components::ui::TextInput>();
    if (!inputsResult) return;

    auto& inputs = inputsResult.value().get();

    for (const auto& entity : inputs.entities()) {
        auto& input = inputs[entity];

        if (input.isFocused) {
            input.blinkTimer += dt;
            if (input.blinkTimer >= input.blinkInterval) {
                input.blinkTimer = 0.0f;
                input.showCursor = !input.showCursor;
            }
        } else {
            input.showCursor = false;
            input.blinkTimer = 0.0f;
        }

        sf::RectangleShape box(sf::Vector2f(input.size.x, input.size.y));
        box.setPosition(sf::Vector2f(input.position.x, input.position.y));
        box.setFillColor(sf::Color(input.bgColor[0], input.bgColor[1], input.bgColor[2], input.alpha));

        const uint8_t* bc = input.isFocused ? input.focusBorderColor : input.borderColor;
        box.setOutlineColor(sf::Color(bc[0], bc[1], bc[2], input.alpha));
        box.setOutlineThickness(2.0f);

        _window.draw(box);

        try {
            sf::Font& font = loadFont(input.fontPath);

            const bool empty = input.value.empty();
            std::string display = empty ? input.placeholder : input.getDisplayValue();

            sf::Text text(font);
            text.setString(sf::String::fromUtf8(display.begin(), display.end()));
            text.setCharacterSize(input.fontSize);
            text.setPosition(sf::Vector2f(input.position.x + 10.0f, input.position.y + 8.0f));

            if (empty) {
                text.setFillColor(sf::Color(input.placeholderColor[0], input.placeholderColor[1], input.placeholderColor[2], input.alpha));
            } else {
                text.setFillColor(sf::Color(input.textColor[0], input.textColor[1], input.textColor[2], input.alpha));
            }

            _window.draw(text);

            if (input.isFocused && input.showCursor) {
                sf::FloatRect bounds = text.getLocalBounds();
                float cursorX = input.position.x + 10.0f + bounds.size.x + 2.0f;
                float cursorY = input.position.y + 10.0f;

                sf::RectangleShape cursor(sf::Vector2f(2.0f, static_cast<float>(input.fontSize)));
                cursor.setPosition(sf::Vector2f(cursorX, cursorY));
                cursor.setFillColor(sf::Color(255, 255, 255, input.alpha));
                _window.draw(cursor);
            }
        } catch (const std::exception& e) {
            rtp::log::error("Failed to render TextInput: {}", e.what());
        }
    }
}

}  // namespace Client::Systems