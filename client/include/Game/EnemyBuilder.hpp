/*
** EPITECH PROJECT, 2025
** Air-Trap, Client
** File description:
** EnemyBuilder
*/

#pragma once

#include <expected>
#include <string>

#include "RType/ECS/Entity.hpp"
#include "RType/ECS/Registry.hpp"
#include "RType/Error.hpp"

#include "RType/Math/Vec2.hpp"

#include "RType/ECS/Components/Transform.hpp"
#include "RType/ECS/Components/Velocity.hpp"
#include "RType/ECS/Components/Sprite.hpp"
#include "RType/ECS/Components/Animation.hpp"

namespace Client::Game {

/**
 * @struct EnemyTemplate
 * @brief Data-driven blueprint used to spawn an enemy entity on the client,
 * incluant une factory statique pour créer des modèles d'ennemis.
 */
struct EnemyTemplate {
    std::string id;                                 /**< Enemy type identifier */

    rtp::Vec2f position{0.0f, 0.0f};                /**< Spawn position */
    float rotation{0.0f};                           /**< Spawn rotation */
    rtp::Vec2f scale{1.0f, 1.0f};                   /**< Spawn scale */

    bool withVelocity{false};                       /**< Whether to add Velocity component */
    rtp::ecs::components::Velocity velocity{};      /**< Initial velocity */

    rtp::ecs::components::Sprite sprite{};          /**< Sprite data */

    bool withAnimation{false};                      /**< Whether to add Animation component */
    rtp::ecs::components::Animation animation{};    /**< Initial animation data */

    static EnemyTemplate createBasicScout(const rtp::Vec2f& initialPos)
    {
        EnemyTemplate t;
        t.id = "basic_scout";

        t.position = initialPos;
        t.rotation = 0.0f;
        t.scale = {1.0f, 1.0f};

        t.withVelocity = true;
        t.velocity.speed = 1.0f;
        t.velocity.direction = {-1.0f, 0.0f};

        t.sprite.texturePath = "assets/sprites/r-typesheet37.gif";
        t.sprite.rectLeft = 0;
        t.sprite.rectTop = 0;
        t.sprite.zIndex = 5;
        t.sprite.red = 255;

        t.withAnimation = true;
        t.animation.frameWidth = 48;
        t.animation.frameHeight = 48;
        t.animation.totalFrames = 4;
        t.animation.frameDuration = 0.1f;
        t.animation.loop = true;
        return t;
    }
};


/**
 * @class EnemyBuilder
 * @brief Spawns enemy entities from an EnemyTemplate.
 */
class EnemyBuilder {
public:
    explicit EnemyBuilder(rtp::ecs::Registry &registry);

    [[nodiscard]]
    auto spawn(const EnemyTemplate &t) -> std::expected<rtp::ecs::Entity, rtp::Error>;

    void kill(rtp::ecs::Entity entity);

    void update(rtp::ecs::Entity entity, const EnemyTemplate &t);

private:
    rtp::ecs::Registry &_registry;
};

} // namespace Client::Game