/*
** EPITECH PROJECT, 2025
** Air-Trap, Client
** File description:
**
 * EnemyBuilder implementation
*/

#include "Game/EntityBuilder.hpp"
#include "Game/SpriteCustomizer.hpp"

namespace rtp::client
{

    EntityBuilder::EntityBuilder(ecs::Registry &registry)
        : _registry(registry)
    {
    }

    auto EntityBuilder::spawn(const EntityTemplate &t)
        -> std::expected<ecs::Entity, Error>
    {
        auto e = _registry.spawn();
        if (!e) {
            return std::unexpected{e.error()};
        }
        auto entity = e.value();

        _registry.add<ecs::components::Transform>(entity, t.position,
                                                       t.rotation, t.scale);

        if (t.withVelocity) {
            _registry.add<ecs::components::Velocity>(entity, t.velocity);
        }

        // Apply custom sprite if exists
        auto sprite = t.sprite;
        auto& customizer = SpriteCustomizer::getInstance();
        
        // Try to map entity name from template tag or common sprite names
        std::string entityKey;
        
        // First, check if template has a specific tag (for bullets, etc.)
        if (!t.tag.empty()) {
            log::debug("EntityBuilder: Checking tag '{}' for entity '{}'", t.tag, t.id);
        }
        
        if (t.tag == "player_bullet") {
            entityKey = "player_shot_6";
            log::info("EntityBuilder: Mapped player_bullet to player_shot_6");
        } else if (t.tag == "enemy_bullet") {
            entityKey = "enemy_shot_6";
            log::info("EntityBuilder: Mapped enemy_bullet to enemy_shot_6");
        } else if (t.tag == "boomerang_bullet") {
            entityKey = "shot_1";
            log::info("EntityBuilder: Mapped boomerang_bullet to shot_1");
        } else if (t.tag == "player_ship") {
            entityKey = "player_ship";
            log::info("EntityBuilder: Mapped player_ship to player_ship");
        } else if (sprite.texturePath.find("r-typesheet1.gif") != std::string::npos) {
            if (sprite.rectLeft == 101 && sprite.rectTop == 3) {
                entityKey = "player_ship";
            } else if (sprite.rectLeft == 134 && sprite.rectTop == 18) {
                entityKey = "shot_insane";
            } else if (sprite.rectLeft == 2 && sprite.rectTop == 51) {
                entityKey = "effect_1";
            } else if (sprite.rectLeft == 215 && sprite.rectTop == 85) {
                entityKey = "shot_1";
            } else if (sprite.rectLeft == 232 && sprite.rectTop == 103) {
                entityKey = "shot_2";
            } else if (sprite.rectLeft == 200 && sprite.rectTop == 121) {
                entityKey = "shot_3";
            } else if (sprite.rectLeft == 168 && sprite.rectTop == 137) {
                entityKey = "shot_4";
            } else if (sprite.rectLeft == 104 && sprite.rectTop == 171) {
                entityKey = "shot_5";
            } else if (sprite.rectLeft == 211 && sprite.rectTop == 276) {
                entityKey = "effect_2";
            } else if (sprite.rectLeft == 72 && sprite.rectTop == 296) {
                entityKey = "effect_3";
            }
        } else if (sprite.texturePath.find("r-typesheet2.gif") != std::string::npos) {
            if (sprite.rectLeft == 159 && sprite.rectTop == 35) {
                entityKey = "enemy_1";
            } else if (sprite.rectLeft == 300 && sprite.rectTop == 58) {
                entityKey = "shot_6";
            } else if (sprite.rectLeft == 300 && sprite.rectTop == 71) {
                entityKey = "enemy_2";
            } else if (sprite.rectLeft == 266 && sprite.rectTop == 94) {
                entityKey = "shot_7";
            } else if (sprite.rectLeft == 101 && sprite.rectTop == 118) {
                entityKey = "effect_4";
            } else if (sprite.rectLeft == 157 && sprite.rectTop == 316) {
                entityKey = "effect_5";
            }
        } else if (sprite.texturePath.find("r-typesheet3.gif") != std::string::npos) {
            // Power-ups from r-typesheet3.gif
            if (sprite.rectLeft == 0 && sprite.rectTop == 0) {
                entityKey = "power_up_heal";
            } else if (sprite.rectLeft == 64 && sprite.rectTop == 0) {
                entityKey = "power_up_double";
            } else if (sprite.rectLeft == 128 && sprite.rectTop == 0) {
                entityKey = "power_up_shield";
            }
        } else if (sprite.texturePath.find("r-typesheet39.gif") != std::string::npos) {
            if (sprite.rectLeft == 33 && sprite.rectTop == 1) {
                entityKey = "boss_ship";
            } else if (sprite.rectLeft == 1 && sprite.rectTop == 72) {
                entityKey = "boss_shield";
            }
        }

        if (!entityKey.empty()) {
            log::info("EntityBuilder: entityKey='{}', hasCustomSprite={}", 
                entityKey, customizer.hasCustomSprite(entityKey));
        }

        bool hasActiveCustomization = false;
        if (!entityKey.empty() && customizer.hasCustomSprite(entityKey)) {
            int left = sprite.rectLeft;
            int top = sprite.rectTop;
            sprite.texturePath = customizer.getSpriteInfo(
                entityKey, 
                sprite.texturePath, 
                left, 
                top,
                sprite.rectWidth,
                sprite.rectHeight
            );
            sprite.rectLeft = left;
            sprite.rectTop = top;
            hasActiveCustomization = true;
        }

        _registry.add<ecs::components::Sprite>(entity, sprite);

        // Only disable animation if we actually applied a custom sprite
        if (t.withAnimation && !hasActiveCustomization) {
            _registry.add<ecs::components::Animation>(entity, t.animation);
        }

        if (t.withParallax) {
            _registry.add<ecs::components::ParallaxLayer>(entity,
                                                               t.parallax);
        }

        return entity;
    }

    void EntityBuilder::kill(ecs::Entity entity)
    {
        _registry.kill(entity);
    }

    void EntityBuilder::update(ecs::Entity entity, const EntityTemplate &t)
    {
        if (auto arr = _registry.get<ecs::components::Transform>(); arr) {
            auto &sa = arr->get();
            if (entity < sa.size() && sa.has(entity)) {
                auto &tr = sa[entity];
                tr.position = t.position;
                tr.rotation = t.rotation;
                tr.scale = t.scale;
            }
        }

        if (t.withVelocity) {
            if (auto arr = _registry.get<ecs::components::Velocity>();
                arr) {
                auto &sa = arr->get();
                if (entity < sa.size() && sa.has(entity)) {
                    sa[entity] = t.velocity;
                }
            }
        }

        if (auto arr = _registry.get<ecs::components::Sprite>(); arr) {
            auto &sa = arr->get();
            if (entity < sa.size() && sa.has(entity)) {
                sa[entity] = t.sprite;
            }
        }

        if (t.withAnimation) {
            if (auto arr = _registry.get<ecs::components::Animation>();
                arr) {
                auto &sa = arr->get();
                if (entity < sa.size() && sa.has(entity)) {
                    sa[entity] = t.animation;
                }
            }
        }
    }
}
