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
#include "RType/ECS/Components/ParallaxLayer.hpp"

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

    bool withParallax{false};                       /**< Whether to add ParallaxLayer component */
    rtp::ecs::components::ParallaxLayer parallax{};/**< Parallax layer data */

    static EntityTemplate createParrallaxLayer1()
    {
        EntityTemplate t;
        t.id = "parallax_layer";

        t.position = {0.0f, 0.0f};
        t.rotation = 0.0f;
        t.scale = {2.0f, 2.0f};

        t.withVelocity = true;
        t.velocity.speed = 10.0f;
        t.velocity.direction = {-50.0f, 0.0f};

        t.sprite.texturePath = "assets/backgrounds/stars_far.png";
        t.sprite.rectLeft = 0;
        t.sprite.rectTop = 0;
        t.sprite.rectWidth = 1280;
        t.sprite.rectHeight = 720;
        t.sprite.zIndex = 5;
        t.sprite.red = 255;
        t.sprite.opacity = 200;

        t.withParallax = true;
        t.parallax.textureWidth = 1280.0f;
        t.parallax.speed = 100.0f;
        return t;
    }

    static EntityTemplate createParrallaxLayer2()
    {
        EntityTemplate t;
        t.id = "parallax_layer";

        t.position = {0.0f, 0.0f};
        t.rotation = 0.0f;
        t.scale = {2.0f, 2.0f};

        t.withVelocity = true;
        t.velocity.speed = 20.0f;
        t.velocity.direction = {-50.0f, 0.0f};

        t.sprite.texturePath = "assets/backgrounds/stars_near.png";
        t.sprite.rectLeft = 0;
        t.sprite.rectTop = 0;
        t.sprite.rectWidth = 1280;
        t.sprite.rectHeight = 720;
        t.sprite.zIndex = 5;
        t.sprite.red = 255;
        t.sprite.opacity = 255;

        t.withParallax = true;
        t.parallax.textureWidth = 1280.0f;
        t.parallax.speed = 300.0f;
        return t;
    }

    static EntityTemplate createBasicScout(const rtp::Vec2f& initialPos)
    {
        EntityTemplate t;
        t.id = "basic_scout";

        t.position = initialPos;
        t.rotation = 0.0f;
        t.scale = {1.0f, 1.0f};

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

    static EntityTemplate createBasicScout2(const rtp::Vec2f& initialPos)
    {
        EntityTemplate t;
        t.id = "basic_scout";

        t.position = initialPos;
        t.rotation = 0.0f;
        t.scale = {1.0f, 1.0f};

        t.withVelocity = true;
        t.velocity.speed = 2.0f;
        t.velocity.direction = {-1.0f, 0.0f};
        
        t.sprite.texturePath = "assets/sprites/r-typesheet2.gif";
        t.sprite.rectLeft = 300;
        t.sprite.rectTop = 341;
        t.sprite.rectWidth = 32;
        t.sprite.rectHeight = 34;
        t.sprite.zIndex = 5;
        t.sprite.red = 255;

        t.withAnimation = true;
        t.animation.frameWidth = 32;
        t.animation.frameHeight = 34;
        t.animation.frameLeft = 300;
        t.animation.frameTop = 341;
        t.animation.totalFrames = 4;
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