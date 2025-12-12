/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** RenderSystem
*/

#pragma once

#include <SFML/Graphics.hpp>

#include "Graphics/AssetManager.hpp"
#include "RType/ECS/Registry.hpp"
#include "RType/ECS/Components/Transform.hpp"
#include "RType/ECS/Components/Sprite.hpp"

namespace Client::Systems {

/**
 * @brief Render system that draws entities having Position + Sprite components.
 */
class RenderSystem {
   public:
    RenderSystem(rtp::ecs::Registry &registry, Graphics::AssetManager &assets)
        : _registry(registry), _assets(assets) {}

    void update(sf::RenderWindow &window, sf::Time delta)
    {
        (void)delta;

        auto sprites   = _registry.getComponents<rtp::ecs::components::Sprite>();
        auto positions = _registry.getComponents<rtp::ecs::components::Transform>();

        const std::size_t n = std::min(positions.size(), sprites.size());
        for (std::size_t i = 0; i < n; ++i) {
            auto &posOpt = positions[i];
            auto &sprOpt = sprites[i];
            if (!posOpt || !sprOpt)
                continue;

            const auto &pos = *posOpt;
            const auto &spr = *sprOpt;

            // Adapte cette ligne à ton AssetManager si la méthode s'appelle différemment.
            // Attendu: getTexture(id) -> sf::Texture& ou const sf::Texture&
            const sf::Texture &tex = _assets.getTexture(spr.textureId);

            sf::Sprite drawable;
            drawable.setTexture(tex, true);
            drawable.setPosition(pos.x, pos.y);
            drawable.setColor(sf::Color(spr.red, spr.green, spr.blue, spr.opacity));

            window.draw(drawable);
        }
    }

   private:
    rtp::ecs::Registry &_registry;
    Graphics::AssetManager &_assets;
};
}  // namespace Client::Systems