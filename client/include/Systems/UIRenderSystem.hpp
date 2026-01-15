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
#include "RType/ECS/Components/UI/SpritePreview.hpp"

namespace rtp::client::systems
{

    class UIRenderSystem : public ecs::ISystem {
        public:
            UIRenderSystem(ecs::Registry& registry, sf::RenderWindow& window);

            void update(float dt) override;
            void clearTextureCache();

        private:
            void renderButtons();
            void renderTexts();
            void renderSliders();
            void renderDropdowns();
            void renderTextInputs(float dt);
            void renderSpritePreviews();
            sf::Font& loadFont(const std::string& fontPath);
            sf::Texture& loadTexture(const std::string& texturePath);

            ecs::Registry& _registry;
            sf::RenderWindow& _window;
            std::unordered_map<std::string, sf::Font> _fonts;
            std::unordered_map<std::string, sf::Texture> _textures;
    };

}  // namespace rtp::client::systems

#endif /* !UIRENDERSYSTEM_HPP_ */