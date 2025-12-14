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
}

void UIRenderSystem::renderButtons() {
    auto buttonsResult = _registry.getComponents<rtp::ecs::components::ui::Button>();
    if (!buttonsResult) return;
    
    auto& buttons = buttonsResult.value().get();
    for (const auto& entity : buttons.getEntities()) {
        auto& button = buttons[entity];
        
        // Create rectangle shape
        sf::RectangleShape rect(sf::Vector2f(button.size.x, button.size.y));
        rect.setPosition(sf::Vector2f(button.position.x, button.position.y));
        
        // Set color based on state
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
        
        // Draw button text
        try {
            sf::Font& font = loadFont("assets/fonts/main.ttf");
            sf::Text text(font, button.text, 24);
            
            // Center text on button
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
    auto textsResult = _registry.getComponents<rtp::ecs::components::ui::Text>();
    if (!textsResult) return;
    
    auto& texts = textsResult.value().get();
    for (const auto& entity : texts.getEntities()) {
        auto& textComp = texts[entity];
        
        try {
            sf::Font& font = loadFont(textComp.fontPath);
            sf::Text text(font, textComp.content, textComp.fontSize);
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
    // Check if font already loaded
    auto it = _fonts.find(fontPath);
    if (it != _fonts.end()) {
        return it->second;
    }
    
    // Load font
    sf::Font font;
    if (!font.openFromFile(fontPath)) {
        throw std::runtime_error("Failed to load font: " + fontPath);
    }
    
    _fonts[fontPath] = std::move(font);
    return _fonts[fontPath];
}

}  // namespace Client::Systems