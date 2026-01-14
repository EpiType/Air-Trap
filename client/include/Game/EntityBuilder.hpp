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

namespace rtp::client {

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

    // Generic sprite + optional animation definition
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
        rtp::Vec2f direction;
        rtp::Vec2f scale{1.0f, 1.0f};
    };

    // Parallax layer definition
    struct ParallaxDef {
        const char *texturePath;
        float velocitySpeed;
        rtp::Vec2f velocityDir;
        int opacity;
        float parallaxSpeed;
        float textureWidth;
        rtp::Vec2f scale{2.0f, 2.0f};
        int rectWidth{1280};
        int rectHeight{720};
    };

    // Helper: build an EntityTemplate from a SpriteAnimDef
    static EntityTemplate makeFromDef(const rtp::Vec2f &initialPos, const SpriteAnimDef &d)
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

    // Helper: build an EntityTemplate for a parallax layer
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
        t.sprite.rectWidth = p.rectWidth;
        t.sprite.rectHeight = p.rectHeight;
        t.sprite.zIndex = -1000;
        t.sprite.red = 255;
        t.sprite.opacity = p.opacity;

        t.withParallax = true;
        t.parallax.textureWidth = p.textureWidth;
        t.parallax.speed = p.parallaxSpeed;
        return t;
    }

    // Data tables
    static constexpr ParallaxDef kParallaxLvl1_1{ "assets/backgrounds/lvl1/stars_far.png", 10.0f, {-50.0f, 0.0f}, 200, 100.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl1_2{ "assets/backgrounds/lvl1/stars_near.png", 20.0f, {-50.0f, 0.0f}, 255, 300.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl2_1{ "assets/backgrounds/lvl2/moon_back.png", 10.0f, {0.0f, 0.0f}, 200, 80.0f, 1200.0f, {1.0f, 1.0f}, 1300, 720 };
    static constexpr ParallaxDef kParallaxLvl2_2{ "assets/backgrounds/lvl2/moon_earth.png", 25.0f, {-30.0f, 0.0f}, 255, 150.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl2_3{ "assets/backgrounds/lvl2/moon_floor.png", 12.0f, {-20.0f, 0.0f}, 200, 60.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl2_4{ "assets/backgrounds/lvl2/moon_front.png", 18.0f, {-40.0f, 0.0f}, 200, 90.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl2_5{ "assets/backgrounds/lvl2/moon_mid.png", 20.0f, {-25.0f, 0.0f}, 255, 70.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl2_6{ "assets/backgrounds/lvl2/moon_sky.png", 10.0f, {-15.0f, 0.0f}, 200, 50.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl3_1{ "assets/backgrounds/lvl3/Sky_back_mountain.png", 15.0f, {-20.0f, 0.0f}, 255, 60.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl3_2{ "assets/backgrounds/lvl3/sky_cloud_floor_2.png", 22.0f, {-35.0f, 0.0f}, 200, 85.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl3_3{ "assets/backgrounds/lvl3/sky_cloud_floor.png", 18.0f, {-25.0f, 0.0f}, 255, 70.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl3_4{ "assets/backgrounds/lvl3/Sky_cloud_single.png", 20.0f, {-30.0f, 0.0f}, 255, 90.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl3_5{ "assets/backgrounds/lvl3/sky_clouds.png", 12.0f, {-15.0f, 0.0f}, 200, 55.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl3_6{ "assets/backgrounds/lvl3/Sky_front_cloud.png", 12.0f, {-15.0f, 0.0f}, 200, 55.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl3_7{ "assets/backgrounds/lvl3/sky_front_mountain.png", 12.0f, {-15.0f, 0.0f}, 200, 55.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl3_8{ "assets/backgrounds/lvl3/sky_moon.png", 12.0f, {-15.0f, 0.0f}, 200, 55.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl3_9{ "assets/backgrounds/lvl3/Sky_sky.png", 12.0f, {-15.0f, 0.0f}, 200, 55.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl4_1{ "assets/backgrounds/lvl4/desert_cloud.png", 15.0f, {-30.0f, 0.0f}, 200, 80.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl4_2{ "assets/backgrounds/lvl4/desert_dunefrontt.png", 25.0f, {-30.0f, 0.0f}, 255, 150.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl4_3{ "assets/backgrounds/lvl4/desert_dunemid.png", 12.0f, {-20.0f, 0.0f}, 200, 60.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl4_4{ "assets/backgrounds/lvl4/desert_moon.png", 18.0f, {-40.0f, 0.0f}, 200, 90.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl4_5{ "assets/backgrounds/lvl4/desert_mountain.png", 20.0f, {-25.0f, 0.0f}, 255, 70.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };
    static constexpr ParallaxDef kParallaxLvl4_6{ "assets/backgrounds/lvl4/desert_sky.png", 10.0f, {-15.0f, 0.0f}, 200, 50.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720 };

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

    static EntityTemplate createParrallaxLvl1_1()
    {
        return makeParallax(kParallaxLvl1_1);
    }

    static EntityTemplate createParrallaxLvl1_2()
    {
        return makeParallax(kParallaxLvl1_2);
    }

    static EntityTemplate createParallaxLvl2_1()
    {
        return makeParallax(kParallaxLvl2_1);
    }

    static EntityTemplate createParallaxLvl2_2()
    {
        return makeParallax(kParallaxLvl2_2);
    }

    static EntityTemplate createParallaxLvl2_3()
    {
        return makeParallax(kParallaxLvl2_3);
    }

    static EntityTemplate createParallaxLvl2_4()
    {
        return makeParallax(kParallaxLvl2_4);
    }

    static EntityTemplate createParallaxLvl2_5()
    {
        return makeParallax(kParallaxLvl2_5);
    }

    static EntityTemplate createParallaxLvl2_6()
    {
        return makeParallax(kParallaxLvl2_6);
    }

    static EntityTemplate createParallaxLvl3_1()
    {
        return makeParallax(kParallaxLvl3_1);
    }

    static EntityTemplate createParallaxLvl3_2()
    {
        return makeParallax(kParallaxLvl3_2);
    }

    static EntityTemplate createParallaxLvl3_3()
    {
        return makeParallax(kParallaxLvl3_3);
    }

    static EntityTemplate createParallaxLvl3_4()
    {
        return makeParallax(kParallaxLvl3_4);
    }

    static EntityTemplate createParallaxLvl3_5()
    {
        return makeParallax(kParallaxLvl3_5);
    }

    static EntityTemplate createParallaxLvl3_6()
    {
        return makeParallax(kParallaxLvl3_6);
    }

    static EntityTemplate createParallaxLvl3_7()
    {
        return makeParallax(kParallaxLvl3_7);
    }

    static EntityTemplate createParallaxLvl3_8()
    {
        return makeParallax(kParallaxLvl3_8);
    }

    static EntityTemplate createParallaxLvl3_9()
    {
        return makeParallax(kParallaxLvl3_9);
    }

    static EntityTemplate createParallaxLvl4_1()
    {
        return makeParallax(kParallaxLvl4_1);
    }

    static EntityTemplate createParallaxLvl4_2()
    {
        return makeParallax(kParallaxLvl4_2);
    }

    static EntityTemplate createParallaxLvl4_3()
    {
        return makeParallax(kParallaxLvl4_3);
    }

    static EntityTemplate createParallaxLvl4_4()
    {
        return makeParallax(kParallaxLvl4_4);
    }

    static EntityTemplate createParallaxLvl4_5()
    {
        return makeParallax(kParallaxLvl4_5);
    }

    static EntityTemplate createParallaxLvl4_6()
    {
        return makeParallax(kParallaxLvl4_6);
    }

    static EntityTemplate rt1_1(const rtp::Vec2f& initialPos)
    {
        return makeFromDef(initialPos, k_rt1_1);
    }

    static EntityTemplate rt1_2(const rtp::Vec2f& initialPos)
    {
        return makeFromDef(initialPos, k_rt1_2);
    }

    static EntityTemplate rt1_3(const rtp::Vec2f& initialPos)
    {
        return makeFromDef(initialPos, k_rt1_3);
    }

    static EntityTemplate rt1_4(const rtp::Vec2f& initialPos)
    {
        return makeFromDef(initialPos, k_rt1_4);
    }

    static EntityTemplate rt1_5(const rtp::Vec2f& initialPos)
    {
        return makeFromDef(initialPos, k_rt1_5);
    }

    static EntityTemplate rt1_6(const rtp::Vec2f& initialPos)
    {
        return makeFromDef(initialPos, k_rt1_6);
    }

    static EntityTemplate rt1_7(const rtp::Vec2f& initialPos)
    {
        return makeFromDef(initialPos, k_rt1_7);
    }

    static EntityTemplate rt1_8(const rtp::Vec2f& initialPos)
    {
        return makeFromDef(initialPos, k_rt1_8);
    }

    static EntityTemplate rt1_12(const rtp::Vec2f& initialPos)
    {
        return makeFromDef(initialPos, k_rt1_12);
    }

    static EntityTemplate rt1_13(const rtp::Vec2f& initialPos)
    {
        return makeFromDef(initialPos, k_rt1_13);
    }

    static EntityTemplate rt2_2(const rtp::Vec2f& initialPos)
    {
        return makeFromDef(initialPos, k_rt2_2);
    }

    static EntityTemplate rt2_3(const rtp::Vec2f& initialPos)
    {
        return makeFromDef(initialPos, k_rt2_3);
    }

    static EntityTemplate rt2_4(const rtp::Vec2f& initialPos)
    {
        return makeFromDef(initialPos, k_rt2_4);
    }

    static EntityTemplate rt2_5(const rtp::Vec2f& initialPos)
    {
        return makeFromDef(initialPos, k_rt2_5);
    }

    static EntityTemplate rt2_6(const rtp::Vec2f& initialPos)
    {
        return makeFromDef(initialPos, k_rt2_6);
    }

    static EntityTemplate rt2_8(const rtp::Vec2f& initialPos)
    {
        return makeFromDef(initialPos, k_rt2_7);
    }

    static EntityTemplate rt2_7(const rtp::Vec2f& initialPos)
    {
        return makeFromDef(initialPos, k_rt2_7);
    }

    // Convenience aliases used by Application.cpp
    static EntityTemplate createBulletEnemy(const rtp::Vec2f& initialPos)
    {
        return makeFromDef(initialPos, k_rt2_3);
    }

    static EntityTemplate createBasicScout2(const rtp::Vec2f& initialPos)
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
    explicit EntityBuilder(rtp::ecs::Registry &registry);

    [[nodiscard]]
    auto spawn(const EntityTemplate &t) -> std::expected<rtp::ecs::Entity, rtp::Error>;

    void kill(rtp::ecs::Entity entity);

    void update(rtp::ecs::Entity entity, const EntityTemplate &t);
private:
    rtp::ecs::Registry &_registry;
};

} // namespace Client::Game