/**
 * File   : RenderSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#ifndef RTYPE_RENDER_SYSTEM_HPP_
    #define RTYPE_RENDER_SYSTEM_HPP_

    #include "RType/ECS/Registry.hpp"
    #include "RType/Logger.hpp"
    #include "RType/ECS/ISystem.hpp"
    
    #include <SFML/Graphics.hpp>
    #include <SFML/System.hpp>
    #include <unordered_map>
    #include <vector>
    #include <algorithm>
    #include <string>

    /* ECS Components */
    #include "RType/ECS/Components/Transform.hpp"
    #include "RType/ECS/Components/Sprite.hpp"

namespace rtp::client {

    class RenderSystem : public rtp::ecs::ISystem {
        public:
            RenderSystem(rtp::ecs::Registry& r, sf::RenderWindow& window) 
                : _r(r), _window(window) {}

            void update(float dt) override {
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
                    const std::string& path = spriteComp.texturePath;
                    
                    if (_textureCache.find(path) == _textureCache.end()) {
                        sf::Texture tex;
                        if (!tex.loadFromFile(path)) {
                            rtp::log::error("TEXTURE ERROR: Impossible de charger '{}'. Vérifie le dossier assets !", path);
                            _textureCache[path] = sf::Texture(); 
                        } else {
                            rtp::log::info("Texture chargée: {}", path);
                            _textureCache[path] = std::move(tex);
                        }
                    } else {
                        rtp::log::info("Texture déjà en cache: {}", path);
                    }

                    sf::Sprite s(_textureCache[path]);
                    bool isTextureValid = (_textureCache[path].getSize().x > 0);

                    if (!isTextureValid) {
                        rtp::log::warning("Using error texture for entity due to invalid texture at path: {}", path);
                        s.setTextureRect(sf::IntRect({0, 0}, {32, 32}));
                        s.setColor(sf::Color::Magenta);
                    } else {
                        if (spriteComp.rectWidth > 0 && spriteComp.rectHeight > 0) {
                            s.setTextureRect(sf::IntRect(
                                {spriteComp.rectLeft, spriteComp.rectTop}, 
                                {spriteComp.rectWidth, spriteComp.rectHeight}
                            ));
                        }
                        rtp::log::info("Applied texture rect: left={}, top={}, width={}, height={}",
                            spriteComp.rectLeft, spriteComp.rectTop,
                            spriteComp.rectWidth, spriteComp.rectHeight);
                        s.setColor(sf::Color(spriteComp.red, spriteComp.green, spriteComp.blue, spriteComp.opacity));
                    }

                    s.setPosition({trans.position.x, trans.position.y});
                    s.setRotation(sf::degrees(trans.rotation)); 
                    s.setScale({trans.scale.x, trans.scale.y});

                    if (isTextureValid && spriteComp.rectWidth > 0) {
                    s.setOrigin({spriteComp.rectWidth / 2.0f, spriteComp.rectHeight / 2.0f});
                    } else {
                    s.setOrigin({16.f, 16.f});
                    }
                    rtp::log::info("Drawing entity at position ({}, {}) with zIndex {}", trans.position.x, trans.position.y, spriteComp.zIndex);
                    itemsToDraw.push_back({spriteComp.zIndex, s});
                }

                if (entityCount == 0) {
                    rtp::log::warning("RenderSystem: Aucune entité à afficher (ZipView vide)");
                }

                std::sort(itemsToDraw.begin(), itemsToDraw.end(), 
                    [](const RenderItem& a, const RenderItem& b) {
                        return a.zIndex < b.zIndex;
                    });

                for (const auto& item : itemsToDraw) {
                    _window.draw(item.sprite);
                }

                _window.display();
            }

        private:
            rtp::ecs::Registry& _r;
            sf::RenderWindow& _window;
            
            std::unordered_map<std::string, sf::Texture> _textureCache;
    };
} // namespace rtp::client

#endif /* !RTYPE_RENDER_SYSTEM_HPP_ */