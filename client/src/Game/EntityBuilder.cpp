/*
** EPITECH PROJECT, 2025
** Air-Trap, Client
** File description:
** EnemyBuilder implementation
*/

#include "Game/EntityBuilder.hpp"
#include "Game/SpriteCustomizer.hpp"

namespace rtp::client {

    EntityBuilder::EntityBuilder(rtp::ecs::Registry &registry)
        : _registry(registry) {}

    auto EntityBuilder::spawn(const EntityTemplate &t)
        -> std::expected<rtp::ecs::Entity, rtp::Error>
    {
        auto e = _registry.spawn();
        if (!e) {
            return std::unexpected{e.error()};
        }
        auto entity = e.value();

        _registry.add<rtp::ecs::components::Transform>(entity, t.position, t.rotation, t.scale);

        if (t.withVelocity) {
            _registry.add<rtp::ecs::components::Velocity>(entity, t.velocity);
        }

        // Apply custom sprite if exists
        auto sprite = t.sprite;
        auto& customizer = SpriteCustomizer::getInstance();
        
        // Try to map entity name from template tag or common sprite names
        std::string entityKey;
        
        // First, check if template has a specific tag (for bullets, etc.)
        if (!t.tag.empty()) {
            rtp::log::debug("EntityBuilder: Checking tag '{}' for entity '{}'", t.tag, t.id);
        }
        
        if (t.tag == "player_bullet") {
            entityKey = "Player_Laser";
            rtp::log::info("EntityBuilder: Mapped player_bullet to Player_Laser");
        } else if (t.tag == "enemy_bullet") {
            entityKey = "Basic_Laser";
            rtp::log::info("EntityBuilder: Mapped enemy_bullet to Basic_Laser");
        } else if (t.tag == "rt1_1") {
            entityKey = "Player_Ship";
            rtp::log::info("EntityBuilder: Mapped rt1_1 to Player_Ship");
        } else if (sprite.texturePath.find("r-typesheet1.gif") != std::string::npos) {
            if (sprite.rectLeft == 101 && sprite.rectTop == 3) {
                entityKey = "Player_Ship";
            } else if (sprite.rectLeft == 134) {
                entityKey = "Enemy_Fighter_1";
            } else if (sprite.rectLeft == 134) {
                entityKey = "Enemy_Fighter_2";
            } else if (sprite.rectLeft == 2 && sprite.rectTop == 51) {
                entityKey = "Enemy_Fighter_3";
            } else if (sprite.rectLeft == 215) {
                entityKey = "Enemy_Ship_1";
            } else if (sprite.rectLeft == 232) {
                entityKey = "Enemy_Ship_2";
            } else if (sprite.rectLeft == 200) {
                entityKey = "Enemy_Ship_3";
            } else if (sprite.rectLeft == 168) {
                entityKey = "Enemy_Ship_4";
            } else if (sprite.rectLeft == 104) {
                entityKey = "Enemy_Ship_5";
            } else if (sprite.rectLeft == 211) {
                entityKey = "Missile_1";
            } else if (sprite.rectLeft == 72) {
                entityKey = "Explosion";
            }
        } else if (sprite.texturePath.find("r-typesheet2.gif") != std::string::npos) {
            if (sprite.rectLeft == 159 && sprite.rectTop == 35) {
                entityKey = "Enemy_Drone";
            } else if (sprite.rectLeft == 300 && sprite.rectTop == 58) {
                entityKey = "Basic_Laser";
            } else if (sprite.rectLeft == 300 && sprite.rectTop == 71) {
                entityKey = "Heavy_Laser";
            } else if (sprite.rectLeft == 266) {
                entityKey = "Plasma_Shot";
            } else if (sprite.rectLeft == 101) {
                entityKey = "Energy_Beam";
            } else if (sprite.rectLeft == 157) {
                entityKey = "Special_Beam";
            }
        }

        if (!entityKey.empty()) {
            rtp::log::info("EntityBuilder: entityKey='{}', hasCustomSprite={}", 
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

        _registry.add<rtp::ecs::components::Sprite>(entity, sprite);

        // Only disable animation if we actually applied a custom sprite
        if (t.withAnimation && !hasActiveCustomization) {
            _registry.add<rtp::ecs::components::Animation>(entity, t.animation);
        }

        if (t.withParallax) {
            _registry.add<rtp::ecs::components::ParallaxLayer>(entity, t.parallax);
        }

        return entity;
    }

    void EntityBuilder::kill(rtp::ecs::Entity entity)
    {
        _registry.kill(entity);
    }

    void EntityBuilder::update(rtp::ecs::Entity entity, const EntityTemplate &t)
    {
        if (auto arr = _registry.getComponents<rtp::ecs::components::Transform>(); arr) {
            auto &sa = arr->get();
            if (entity < sa.size() && sa.has(entity)) {
                auto &tr = sa[entity];
                tr.position = t.position;
                tr.rotation = t.rotation;
                tr.scale = t.scale;
            }
        }

        if (t.withVelocity) {
            if (auto arr = _registry.getComponents<rtp::ecs::components::Velocity>(); arr) {
                auto &sa = arr->get();
                if (entity < sa.size() && sa.has(entity)) {
                    sa[entity] = t.velocity;
                }
            }
        }

        if (auto arr = _registry.getComponents<rtp::ecs::components::Sprite>(); arr) {
            auto &sa = arr->get();
            if (entity < sa.size() && sa.has(entity)) {
                sa[entity] = t.sprite;
            }
        }

        if (t.withAnimation) {
            if (auto arr = _registry.getComponents<rtp::ecs::components::Animation>(); arr) {
                auto &sa = arr->get();
                if (entity < sa.size() && sa.has(entity)) {
                    sa[entity] = t.animation;
                }
            }
        }
    }
} // namespace Client::Game
