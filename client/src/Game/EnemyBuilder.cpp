/*
** EPITECH PROJECT, 2025
** Air-Trap, Client
** File description:
** EnemyBuilder implementation
*/

#include "Game/EnemyBuilder.hpp"

namespace Client::Game {

EnemyBuilder::EnemyBuilder(rtp::ecs::Registry &registry)
    : _registry(registry) {}

auto EnemyBuilder::spawn(const EnemyTemplate &t)
    -> std::expected<rtp::ecs::Entity, rtp::Error>
{
    auto e = _registry.spawnEntity();
    if (!e) {
        return std::unexpected{e.error()};
    }
    auto entity = e.value();

    _registry.addComponent<rtp::ecs::components::Transform>(entity, t.position, t.rotation, t.scale);

    if (t.withVelocity) {
        _registry.addComponent<rtp::ecs::components::Velocity>(entity, t.velocity);
    }

    _registry.addComponent<rtp::ecs::components::Sprite>(entity, t.sprite);

    if (t.withAnimation) {
        _registry.addComponent<rtp::ecs::components::Animation>(entity, t.animation);
    }

    return entity;
}

void EnemyBuilder::kill(rtp::ecs::Entity entity)
{
    _registry.killEntity(entity);
}

void EnemyBuilder::update(rtp::ecs::Entity entity, const EnemyTemplate &t)
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
