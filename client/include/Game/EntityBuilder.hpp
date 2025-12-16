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
struct EntityTemplate {
    std::string id;                                 /**< Enemy type identifier */

    rtp::Vec2f position{0.0f, 0.0f};                /**< Spawn position */
    float rotation{0.0f};                           /**< Spawn rotation */
    rtp::Vec2f scale{1.0f, 1.0f};                   /**< Spawn scale */

    bool withVelocity{false};                       /**< Whether to add Velocity component */
    rtp::ecs::components::Velocity velocity{};      /**< Initial velocity */

    rtp::ecs::components::Sprite sprite{};          /**< Sprite data */

    bool withAnimation{false};                      /**< Whether to add Animation component */
    rtp::ecs::components::Animation animation{};    /**< Initial animation data */

    static EntityTemplate createBasicScout(const rtp::Vec2f& initialPos)
    {
        EntityTemplate t;
        t.id = "basic_scout";

        t.position = initialPos;
        t.rotation = 0.0f;
        t.scale = {2.0f, 2.0f};

        t.withVelocity = true;
        t.velocity.speed = 2.0f;
        t.velocity.direction = {-1.0f, 0.0f};
        
        t.sprite.texturePath = "assets/sprites/r-typesheet2.gif";
        t.sprite.rectLeft = 300;
        t.sprite.rectTop = 71;
        t.sprite.rectWidth = 30;
        t.sprite.rectHeight = 18;
        t.sprite.zIndex = 5;
        t.sprite.red = 255;

        t.withAnimation = true;
        t.animation.frameWidth = 30;
        t.animation.frameHeight = 18;
        t.animation.frameLeft = 300;
        t.animation.frameTop = 71;
        t.animation.totalFrames = 6;
        return t;
    }

    static EntityTemplate createBulletEnemy(const rtp::Vec2f& initialPos)
    {
        EntityTemplate t;
        t.id = "bullet_enemy";

        t.position = initialPos;
        t.rotation = 0.0f;
        t.scale = {1.0f, 1.0f};

        t.withVelocity = true;
        t.velocity.speed = 50.0f;
        t.velocity.direction = {-1.0f, 0.0f};

        t.sprite.texturePath = "assets/sprites/r-typesheet2.gif";
        t.sprite.rectLeft = 300;
        t.sprite.rectTop = 58;
        t.sprite.rectWidth = 18;
        t.sprite.rectHeight = 6;
        t.sprite.zIndex = 5;
        t.sprite.red = 255;

        t.withAnimation = false;

        return t;
    }
};


/**
 * @class EnemyBuilder
 * @brief Spawns enemy entities from an EnemyTemplate.
 */
class EntityBuilder {
public:
    explicit EntityBuilder(rtp::ecs::Registry &registry);

    [[nodiscard]]
    auto spawn(const EntityTemplate &t) -> std::expected<rtp::ecs::Entity, rtp::Error>;

    void kill(rtp::ecs::Entity entity);

    void update(rtp::ecs::Entity entity, const EntityTemplate &t);
private:
    rtp::ecs::Registry &_registry;
};

} // namespace Client::Game