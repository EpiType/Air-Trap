/*
** EPITECH PROJECT, 2025
** Air-Trap, Client
** File description:
**
 * EnemyBuilder implementation
*/

#include "Game/EntityBuilder.hpp"

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

        _registry.add<ecs::components::Sprite>(entity, t.sprite);

        if (t.withAnimation) {
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
