/**
 * File   : RenderSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/12/2025
 */

#include "Systems/RenderSystem.hpp"
#include "Mod/ModManager.hpp"

namespace rtp::client {

void RenderSystem::update(float dt) {
    (void)dt;
    _window.clear(sf::Color(20, 20, 30));

    struct RenderItem {
        int zIndex;
        sf::Sprite sprite;
    };
    std::vector<RenderItem> itemsToDraw;

    auto view = _r.zipView<rtp::ecs::components::Transform, rtp::ecs::components::Sprite>();
    
    int entityCount = 0;

    for (auto&& [trans, spriteComp] : view) {
        entityCount++;
        const std::string& originalPath = spriteComp.texturePath;
        
        // Get effective path (modded or original)
        std::string path = originalPath;
        if (_modManager) {
            path = _modManager->getEffectivePathByRect(
                originalPath,
                spriteComp.rectLeft,
                spriteComp.rectTop,
                spriteComp.rectWidth,
                spriteComp.rectHeight
            );
        }
        
        if (_textureCache.find(path) == _textureCache.end()) {
            sf::Texture tex;
            if (!tex.loadFromFile(path)) {
                rtp::log::error("TEXTURE ERROR: Impossible de charger '{}'. Vérifie le dossier assets !", path);
                _textureCache[path] = sf::Texture(); 
            } else {
                rtp::log::info("✓ Texture loaded: '{}' size={}x{}{}", 
                             path, tex.getSize().x, tex.getSize().y,
                             (path != originalPath ? " [MOD]" : ""));
                _textureCache[path] = std::move(tex);
            }
        }

        sf::Sprite s(_textureCache[path]);
        bool isTextureValid = (_textureCache[path].getSize().x > 0);
        bool isModdedSprite = (path != originalPath);

        if (!isTextureValid) {
            rtp::log::warning("Using error texture for entity due to invalid texture at path: {}", path);
            s.setTextureRect(sf::IntRect({0, 0}, {32, 32}));
            s.setColor(sf::Color::Magenta);
        } else {
            if (isModdedSprite) {
                // Modded sprites are standalone images, use full texture
                auto size = _textureCache[path].getSize();
                s.setTextureRect(sf::IntRect({0, 0}, {static_cast<int>(size.x), static_cast<int>(size.y)}));
                s.setScale({trans.scale.x, trans.scale.y});
                s.setOrigin({size.x / 2.0f, size.y / 2.0f});
            } else if (spriteComp.rectWidth > 0 && spriteComp.rectHeight > 0) {
                // Original spritesheet sprites
                s.setTextureRect(sf::IntRect(
                    {spriteComp.rectLeft, spriteComp.rectTop}, 
                    {spriteComp.rectWidth, spriteComp.rectHeight}
                ));
                s.setScale({trans.scale.x, trans.scale.y});
                s.setOrigin({spriteComp.rectWidth / 2.0f, spriteComp.rectHeight / 2.0f});
            } else {
                s.setScale({trans.scale.x, trans.scale.y});
                s.setOrigin({16.f, 16.f});
            }
            s.setColor(sf::Color(spriteComp.red, spriteComp.green, spriteComp.blue, spriteComp.opacity));
        }

        s.setPosition({trans.position.x, trans.position.y});
        s.setRotation(sf::degrees(trans.rotation));
        
        itemsToDraw.push_back({spriteComp.zIndex, s});
    }

    std::sort(itemsToDraw.begin(), itemsToDraw.end(), 
        [](const RenderItem& a, const RenderItem& b) {
            return a.zIndex < b.zIndex;
        });

    // Draw all sprites normally
    for (const auto& item : itemsToDraw) {
        _window.draw(item.sprite);
    }
}

} // namespace rtp::client

