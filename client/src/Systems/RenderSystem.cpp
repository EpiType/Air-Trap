/**
 * File   : RenderSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/12/2025
 */

#include "Systems/RenderSystem.hpp"
#include "Utils/DebugFlags.hpp"

namespace rtp::client {

void RenderSystem::update(float dt)
{
    (void)dt;

    struct RenderItem {
        int zIndex;
        sf::Sprite sprite;
    };
    std::vector<RenderItem> itemsToDraw;

    auto view = _r.zipView<ecs::components::Transform, ecs::components::Sprite>();

    for (auto&& [trans, spriteComp] : view) {
        const std::string &path = spriteComp.texturePath;

        if (_textureCache.find(path) == _textureCache.end()) {
            sf::Texture tex;
            if (!tex.loadFromFile(path)) {
                log::error("TEXTURE ERROR: Impossible de charger '{}'", path);
                _textureCache[path] = sf::Texture();
            } else {
                _textureCache[path] = std::move(tex);
            }
        }

        sf::Sprite s(_textureCache[path]);
        const bool valid = (_textureCache[path].getSize().x > 0);

        if (!valid) {
            s.setTextureRect(sf::IntRect({0, 0}, {32, 32}));
            s.setColor(sf::Color::Magenta);
        } else {
            if (spriteComp.rectWidth > 0 && spriteComp.rectHeight > 0) {
                s.setTextureRect(sf::IntRect(
                    {spriteComp.rectLeft, spriteComp.rectTop},
                    {spriteComp.rectWidth, spriteComp.rectHeight}
                ));
            }
            s.setColor(sf::Color(spriteComp.red, spriteComp.green, spriteComp.blue, spriteComp.opacity));
        }

        s.setPosition({trans.position.x, trans.position.y});
        s.setRotation(sf::degrees(trans.rotation));
        s.setScale({trans.scale.x, trans.scale.y});
        s.setOrigin(valid && spriteComp.rectWidth > 0
                        ? sf::Vector2f(spriteComp.rectWidth / 2.0f, spriteComp.rectHeight / 2.0f)
                        : sf::Vector2f(16.f, 16.f));

        itemsToDraw.push_back({spriteComp.zIndex, s});
    }

    std::sort(itemsToDraw.begin(), itemsToDraw.end(),
        [](const RenderItem& a, const RenderItem& b) { return a.zIndex < b.zIndex; });

    for (const auto &item : itemsToDraw) {
        _window.draw(item.sprite);
        if (rtp::client::g_drawDebugBounds) {
            sf::FloatRect bounds = item.sprite.getGlobalBounds();
            sf::RectangleShape box({bounds.size.x, bounds.size.y});
            box.setPosition({bounds.position.x, bounds.position.y});
            box.setFillColor(sf::Color::Transparent);
            box.setOutlineColor(sf::Color(255, 80, 80, 200));
            box.setOutlineThickness(1.0f);
            _window.draw(box);
        }
    }

    auto obstacleView = _r.zipView<
        ecs::components::Transform,
        ecs::components::BoundingBox,
        ecs::components::EntityType
    >();

    for (auto&& [trans, box, type] : obstacleView) {
        if (type.type != net::EntityType::Obstacle &&
            type.type != net::EntityType::ObstacleSolid) {
            continue;
        }

        sf::RectangleShape rect({box.width, box.height});
        rect.setPosition({trans.position.x, trans.position.y});
        if (type.type == net::EntityType::ObstacleSolid) {
            rect.setFillColor(sf::Color(40, 40, 40, 220));
            rect.setOutlineColor(sf::Color(90, 90, 90, 240));
        } else {
            rect.setFillColor(sf::Color(60, 60, 60, 180));
            rect.setOutlineColor(sf::Color(120, 120, 120, 220));
        }
        rect.setOutlineThickness(1.0f);
        _window.draw(rect);
    }
}

} // namespace rtp::client
