/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** UIRenderSystem
*/

#ifndef UIRENDERSYSTEM_HPP_
#define UIRENDERSYSTEM_HPP_

#include <SFML/Graphics.hpp>
#include <unordered_map>
#include "RType/ECS/Registry.hpp"
#include "RType/ECS/ISystem.hpp"
#include "RType/ECS/Components/UI/Button.hpp"
#include "RType/ECS/Components/UI/Text.hpp"
#include "RType/ECS/Components/UI/Slider.hpp"
#include "RType/ECS/Components/UI/Dropdown.hpp"
#include "RType/ECS/Components/UI/TextInput.hpp"

namespace Client::Systems {

class UIRenderSystem : public rtp::ecs::ISystem {
public:
    UIRenderSystem(rtp::ecs::Registry& registry, sf::RenderWindow& window);

    void update(float dt) override;

private:
    void renderButtons();
    void renderTexts();
    void renderSliders();
    void renderDropdowns();
    void renderTextInputs(float dt);
    sf::Font& loadFont(const std::string& fontPath);

    rtp::ecs::Registry& _registry;
    sf::RenderWindow& _window;
    std::unordered_map<std::string, sf::Font> _fonts;
};

}  // namespace Client::Systems

#endif /* !UIRENDERSYSTEM_HPP_ */