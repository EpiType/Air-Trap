/*
** EPITECH PROJECT, 2025
** Air-Trap, Client
** File description:
** EnemyBuilder
*/

#ifndef RTYPE_CLIENT_ENTITY_ENTITYBUILDER_HPP_
    #define RTYPE_CLIENT_ENTITY_ENTITYBUILDER_HPP_

    #include <expected>
    #include <string>

    /* Engine */
    #include "engine/ecs/Entity.hpp"
    #include "engine/ecs/Registry.hpp"
    #include "engine/core/Error.hpp"
    #include "engine/math/Vec2.hpp"
    #include "engine/ecs/components/ImportAllEngineComponents.hpp"

    /* R-Type Client */
    #include "rtype/components/ParallaxLayer.hpp"

namespace rtp::client
{
    /**
     * @struct EnemyTemplate
     * @brief Data-driven blueprint used to spawn an enemy entity on the client,
     * incluant une factory statique pour créer des modèles d'ennemis.
     */
    struct EntityTemplate {
        std::string id;                                     /**< Enemy type identifier */

        engine::math::Vec2f position{0.0f, 0.0f};           /**< Spawn position */
        float rotation{0.0f};                               /**< Spawn rotation */
        engine::math::Vec2f scale{1.0f, 1.0f};              /**< Spawn scale */

        bool withVelocity{false};                           /**< Whether to add Velocity component */
        engine::ecs::components::Velocity velocity{};       /**< Initial velocity */

        engine::ecs::components::Sprite sprite{};           /**< Sprite data */

        bool withAnimation{false};                          /**< Whether to add Animation component */
        engine::ecs::components::Animation animation{};     /**< Initial animation data */

        bool withParallax{false};                           /**< Whether to add ParallaxLayer component */
        rtp::ecs::components::ParallaxLayer parallax{};     /**< Parallax layer data */

        struct SpriteAnimDef {
            const char *id;
            const char *texturePath;
            int rectLeft;
            int rectTop;
            int rectWidth;
            int rectHeight;
            int zIndex;
            int red;
            bool withAnimation;
            int frameWidth;
            int frameHeight;
            int frameLeft;
            int frameTop;
            int totalFrames;
            float speed;
            engine::math::Vec2f direction;
            engine::math::Vec2f scale{1.0f, 1.0f};
        };

        struct ParallaxDef {
            const char *texturePath;
            float velocitySpeed;
            engine::math::Vec2f velocityDir;
            int opacity;
            float parallaxSpeed;
            float textureWidth;
            engine::math::Vec2f scale{2.0f, 2.0f};
        };

        static EntityTemplate makeFromDef(const engine::math::Vec2f &initialPos, const SpriteAnimDef &d)
        {
            EntityTemplate t;
            t.id = d.id;
            t.position = initialPos;
            t.rotation = 0.0f;
            t.scale = d.scale;

            t.withVelocity = true;
            t.velocity.speed = d.speed;
            t.velocity.direction = d.direction;

            t.sprite.texturePath = d.texturePath;
            t.sprite.rectLeft = d.rectLeft;
            t.sprite.rectTop = d.rectTop;
            t.sprite.rectWidth = d.rectWidth;
            t.sprite.rectHeight = d.rectHeight;
            t.sprite.zIndex = d.zIndex;
            t.sprite.red = d.red;

            t.withAnimation = d.withAnimation;
            if (d.withAnimation) {
                t.animation.frameWidth = d.frameWidth;
                t.animation.frameHeight = d.frameHeight;
                t.animation.frameLeft = d.frameLeft;
                t.animation.frameTop = d.frameTop;
                t.animation.totalFrames = d.totalFrames;
            }
            return t;
        }

        static EntityTemplate makeParallax(const ParallaxDef &p)
        {
            EntityTemplate t;
            t.id = "parallax_layer";
            t.position = {0.0f, 0.0f};
            t.rotation = 0.0f;
            t.scale = p.scale;

            t.withVelocity = true;
            t.velocity.speed = p.velocitySpeed;
            t.velocity.direction = p.velocityDir;

            t.sprite.texturePath = p.texturePath;
            t.sprite.rectLeft = 0;
            t.sprite.rectTop = 0;
            t.sprite.rectWidth = 1280;
            t.sprite.rectHeight = 720;
            t.sprite.zIndex = -1000;
            t.sprite.red = 255;
            t.sprite.opacity = p.opacity;

            t.withParallax = true;
            t.parallax.textureWidth = p.textureWidth;
            t.parallax.speed = p.parallaxSpeed;
            return t;
        }

        static constexpr ParallaxDef kParallaxFar{ "assets/backgrounds/stars_far.png", 10.0f, {-50.0f, 0.0f}, 200, 100.0f, 1280.0f, {2.0f, 2.0f} };
        static constexpr ParallaxDef kParallaxNear{ "assets/backgrounds/stars_near.png", 20.0f, {-50.0f, 0.0f}, 255, 300.0f, 1280.0f, {2.0f, 2.0f} };

        static constexpr SpriteAnimDef k_rt1_1{ "rt1_1", "assets/sprites/r-typesheet1.gif", 101, 3, 33, 14, 5, 255, true, 33, 14, 101, 3, 5, 2.0f, {-1.0f, 0.0f}, {1.0f, 1.0f} };
        static constexpr SpriteAnimDef k_rt1_2{ "rt1_2", "assets/sprites/r-typesheet1.gif", 134, 18, 33, 32, 5, 255, true, 33, 32, 134, 18, 4, 2.0f, {-1.0f, 0.0f}, {1.0f, 1.0f} };
        static constexpr SpriteAnimDef k_rt1_3{ "rt1_3", "assets/sprites/r-typesheet1.gif", 2, 51, 33, 32, 5, 255, true, 33, 32, 2, 51, 8, 2.0f, {-1.0f, 0.0f}, {1.0f, 1.0f} };
        static constexpr SpriteAnimDef k_rt1_4{ "rt1_4", "assets/sprites/r-typesheet1.gif", 215, 85, 18, 12, 5, 255, true, 18, 12, 215, 85, 3, 2.0f, {-1.0f, 0.0f}, {1.0f, 1.0f} };
        static constexpr SpriteAnimDef k_rt1_5{ "rt1_5", "assets/sprites/r-typesheet1.gif", 232, 103, 17, 12, 5, 255, true, 17, 12, 232, 103, 2, 2.0f, {-1.0f, 0.0f}, {1.0f, 1.0f} };
        static constexpr SpriteAnimDef k_rt1_6{ "rt1_6", "assets/sprites/r-typesheet1.gif", 200, 121, 33, 10, 5, 255, true, 33, 10, 200, 121, 2, 2.0f, {-1.0f, 0.0f}, {1.0f, 1.0f} };
        static constexpr SpriteAnimDef k_rt1_7{ "rt1_7", "assets/sprites/r-typesheet1.gif", 168, 137, 49, 12, 5, 255, true, 49, 12, 168, 137, 2, 2.0f, {-1.0f, 0.0f}, {1.0f, 1.0f} };
        static constexpr SpriteAnimDef k_rt1_8{ "rt1_8", "assets/sprites/r-typesheet1.gif", 104, 171, 81, 14, 5, 255, true, 81, 14, 104, 171, 2, 2.0f, {-1.0f, 0.0f}, {1.0f, 1.0f} };
        static constexpr SpriteAnimDef k_rt1_12{ "rt1_12", "assets/sprites/r-typesheet1.gif", 211, 276, 16, 12, 5, 255, true, 16, 12, 211, 276, 4, 2.0f, {-1.0f, 0.0f}, {1.0f, 1.0f} };
        static constexpr SpriteAnimDef k_rt1_13{ "rt1_13", "assets/sprites/r-typesheet1.gif", 72, 296, 37, 30, 5, 255, true, 37, 30, 72, 296, 6, 2.0f, {-1.0f, 0.0f}, {1.0f, 1.0f} };

        static constexpr SpriteAnimDef k_rt2_2{ "rt2_2", "assets/sprites/r-typesheet2.gif", 159, 35, 24, 16, 5, 255, true, 24, 16, 159, 35, 6, 2.0f, {-1.0f, 0.0f}, {1.0f, 1.0f} };
        static constexpr SpriteAnimDef k_rt2_3{ "rt2_3", "assets/sprites/r-typesheet2.gif", 300, 58, 18, 6, 5, 255, false, 0, 0, 0, 0, 0, 50.0f, {-1.0f, 0.0f}, {1.0f, 1.0f} };
        static constexpr SpriteAnimDef k_rt2_4{ "rt2_4", "assets/sprites/r-typesheet2.gif", 300, 71, 30, 18, 5, 255, true, 30, 18, 300, 71, 6, 2.0f, {-1.0f, 0.0f}, {1.0f, 1.0f} };
        static constexpr SpriteAnimDef k_rt2_5{ "rt2_5", "assets/sprites/r-typesheet2.gif", 266, 94, 17, 10, 5, 255, true, 17, 10, 266, 94, 2, 2.0f, {-1.0f, 0.0f}, {1.0f, 1.0f} };
        static constexpr SpriteAnimDef k_rt2_6{ "rt2_6", "assets/sprites/r-typesheet2.gif", 101, 118, 17, 14, 5, 255, true, 17, 14, 101, 118, 4, 2.0f, {-1.0f, 0.0f}, {1.0f, 1.0f} };
        static constexpr SpriteAnimDef k_rt2_7{ "rt2_8", "assets/sprites/r-typesheet2.gif", 157, 316, 18, 14, 5, 255, true, 18, 14, 157, 316, 8, 2.0f, {-1.0f, 0.0f}, {1.0f, 1.0f} };

        static EntityTemplate createParrallaxLayer1()
        {
            return makeParallax(kParallaxFar);
        }

        static EntityTemplate createParrallaxLayer2()
        {
            return makeParallax(kParallaxNear);
        }

        static EntityTemplate rt1_1(const engine::math::Vec2f& initialPos)
        {
            return makeFromDef(initialPos, k_rt1_1);
        }

        static EntityTemplate rt1_2(const engine::math::Vec2f& initialPos)
        {
            return makeFromDef(initialPos, k_rt1_2);
        }

        static EntityTemplate rt1_3(const engine::math::Vec2f& initialPos)
        {
            return makeFromDef(initialPos, k_rt1_3);
        }

        static EntityTemplate rt1_4(const engine::math::Vec2f& initialPos)
        {
            return makeFromDef(initialPos, k_rt1_4);
        }

        static EntityTemplate rt1_5(const engine::math::Vec2f& initialPos)
        {
            return makeFromDef(initialPos, k_rt1_5);
        }

        static EntityTemplate rt1_6(const engine::math::Vec2f& initialPos)
        {
            return makeFromDef(initialPos, k_rt1_6);
        }

        static EntityTemplate rt1_7(const engine::math::Vec2f& initialPos)
        {
            return makeFromDef(initialPos, k_rt1_7);
        }

        static EntityTemplate rt1_8(const engine::math::Vec2f& initialPos)
        {
            return makeFromDef(initialPos, k_rt1_8);
        }

        static EntityTemplate rt1_12(const engine::math::Vec2f& initialPos)
        {
            return makeFromDef(initialPos, k_rt1_12);
        }

        static EntityTemplate rt1_13(const engine::math::Vec2f& initialPos)
        {
            return makeFromDef(initialPos, k_rt1_13);
        }

        static EntityTemplate rt2_2(const engine::math::Vec2f& initialPos)
        {
            return makeFromDef(initialPos, k_rt2_2);
        }

        static EntityTemplate rt2_3(const engine::math::Vec2f& initialPos)
        {
            return makeFromDef(initialPos, k_rt2_3);
        }

        static EntityTemplate rt2_4(const engine::math::Vec2f& initialPos)
        {
            return makeFromDef(initialPos, k_rt2_4);
        }

        static EntityTemplate rt2_5(const engine::math::Vec2f& initialPos)
        {
            return makeFromDef(initialPos, k_rt2_5);
        }

        static EntityTemplate rt2_6(const engine::math::Vec2f& initialPos)
        {
            return makeFromDef(initialPos, k_rt2_6);
        }

        static EntityTemplate rt2_8(const engine::math::Vec2f& initialPos)
        {
            return makeFromDef(initialPos, k_rt2_7);
        }

        static EntityTemplate rt2_7(const engine::math::Vec2f& initialPos)
        {
            return makeFromDef(initialPos, k_rt2_7);
        }

        static EntityTemplate createBulletEnemy(const engine::math::Vec2f& initialPos)
        {
            return makeFromDef(initialPos, k_rt2_3);
        }

        static EntityTemplate createBasicScout2(const engine::math::Vec2f& initialPos)
        {
            return makeFromDef(initialPos, k_rt2_2);
        }

    };

    /**
     * @class EnemyBuilder
     * @brief Spawns enemy entities from an EnemyTemplate.
     */
    class EntityBuilder {
    public:
        explicit EntityBuilder(engine::ecs::Registry &registry);

        [[nodiscard]]
        auto spawn(const EntityTemplate &t) -> std::expected<engine::ecs::Entity, engine::core::Error>;

        void kill(engine::ecs::Entity entity);

        void update(engine::ecs::Entity entity, const EntityTemplate &t);
    private:
        engine::ecs::Registry &_registry;
    };
}

#endif /* !RTYPE_CLIENT_ENTITY_ENTITYBUILDER_HPP_ */