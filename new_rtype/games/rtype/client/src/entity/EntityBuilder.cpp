/*
** EPITECH PROJECT, 2025
** Air-Trap, Client
** File description:
** EnemyBuilder implementation
*/

#include "rtype/entity/EntityBuilder.hpp"

namespace rtp::client {

    EntityBuilder::EntityBuilder(aer::ecs::Registry &registry)
        : _registry(registry) {}

    auto EntityBuilder::spawn(const EntityTemplate &t)
        -> std::expected<aer::ecs::Entity, aer::log::Error>
    {
        auto e = _registry.spawn();
        if (!e) {
            return std::unexpected{e.error()};
        }
        auto entity = e.value();

        _registry.add<aer::ecs::components::Transform>(
            entity,
            aer::ecs::components::Transform{t.position, t.scale, t.rotation});

        if (t.withVelocity) {
            _registry.add<aer::ecs::components::Velocity>(entity, t.velocity);
        }

        _registry.add<aer::ecs::components::Sprite>(entity, t.sprite);

        if (t.withAnimation) {
            _registry.add<aer::ecs::components::Animation>(entity, t.animation);
        }

        if (t.withParallax) {
            _registry.add<rtp::ecs::components::ParallaxLayer>(entity, t.parallax);
        }

        return entity;
    }

    void EntityBuilder::kill(aer::ecs::Entity entity)
    {
        _registry.kill(entity);
    }

    void EntityBuilder::update(aer::ecs::Entity entity, const EntityTemplate &t)
    {
        if (auto arr = _registry.getComponents<aer::ecs::components::Transform>(); arr) {
            auto &sa = arr->get();
            if (entity < sa.size() && sa.has(entity)) {
                auto &tr = sa[entity];
                tr.position = t.position;
                tr.rotation = t.rotation;
                tr.scale = t.scale;
            }
        }

        if (t.withVelocity) {
            if (auto arr = _registry.getComponents<aer::ecs::components::Velocity>(); arr) {
                auto &sa = arr->get();
                if (entity < sa.size() && sa.has(entity)) {
                    sa[entity] = t.velocity;
                }
            }
        }

        if (auto arr = _registry.getComponents<aer::ecs::components::Sprite>(); arr) {
            auto &sa = arr->get();
            if (entity < sa.size() && sa.has(entity)) {
                sa[entity] = t.sprite;
            }
        }

        if (t.withAnimation) {
            if (auto arr = _registry.getComponents<aer::ecs::components::Animation>(); arr) {
                auto &sa = arr->get();
                if (entity < sa.size() && sa.has(entity)) {
                    sa[entity] = t.animation;
                }
            }
        }
    }
} // namespace Client::Game
