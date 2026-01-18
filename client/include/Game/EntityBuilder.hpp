/*
** EPITECH PROJECT, 2025
** Air-Trap, Client
** File description:
**
 * EnemyBuilder
*/

#pragma once

#include "RType/ECS/Components/Animation.hpp"
#include "RType/ECS/Components/ParallaxLayer.hpp"
#include "RType/ECS/Components/Sprite.hpp"
#include "RType/ECS/Components/Transform.hpp"
#include "RType/ECS/Components/Velocity.hpp"
#include "RType/ECS/Entity.hpp"
#include "RType/ECS/Registry.hpp"
#include "RType/Error.hpp"
#include "RType/Math/Vec2.hpp"

#include <expected>
#include <string>

namespace rtp::client
{

    /**
     * @struct EnemyTemplate
     * @brief Data-driven blueprint used to spawn an enemy entity on the client,
     * incluant une factory statique pour créer des modèles d'ennemis.
     */
    struct EntityTemplate {
        std::string id; /**< Enemy type identifier */
        std::string tag{}; /**< Template tag for entity differentiation */

        Vec2f position{0.0f, 0.0f}; /**< Spawn position */
        float rotation{0.0f};       /**< Spawn rotation */
        Vec2f scale{1.0f, 1.0f};    /**< Spawn scale */

        bool withVelocity{false}; /**< Whether to add Velocity component */
        ecs::components::Velocity velocity{}; /**< Initial velocity */

        ecs::components::Sprite sprite{}; /**< Sprite data */

        bool withAnimation{
            false}; /**< Whether to add Animation component */
        ecs::components::Animation
            animation{}; /**< Initial animation data */

        bool withParallax{
            false}; /**< Whether to add ParallaxLayer component */
        ecs::components::ParallaxLayer
            parallax{}; /**< Parallax layer data */

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
            int green;
            int blue;
            bool withAnimation;
            int frameWidth;
            int frameHeight;
            int frameLeft;
            int frameTop;
            int totalFrames;
            float speed;
            Vec2f direction;
            Vec2f scale{1.0f, 1.0f};
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
        float posY{0.0f};
    };

        // Helper: build an EntityTemplate from a SpriteAnimDef
        static EntityTemplate makeFromDef(const Vec2f &initialPos,
                                            const SpriteAnimDef &d)
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
            t.sprite.green = d.green;
            t.sprite.blue = d.blue;

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
            t.position = {0.0f, p.posY};
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
    static constexpr ParallaxDef kParallaxLvl1_1{ "assets/backgrounds/lvl1/stars_far.png", 10.0f, {-50.0f, 0.0f}, 200, 100.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720, 0.0f };
    static constexpr ParallaxDef kParallaxLvl1_2{ "assets/backgrounds/lvl1/stars_near.png", 20.0f, {-50.0f, 0.0f}, 255, 300.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720, 0.0f };
    static constexpr ParallaxDef kParallaxLvl2_1{ "assets/backgrounds/lvl2/moon_back.png", 10.0f, {-1.0f, 0.0f}, 200, 80.0f, 3800.0f, {0.72f, 0.72f}, 3800, 1000, 360.0f };
    static constexpr ParallaxDef kParallaxLvl2_2{ "assets/backgrounds/lvl2/moon_earth.png", 10.0f, {-1.0f, 0.0f}, 200, 80.0f, 3800.0f, {0.72f, 0.72f}, 3800, 1000, 360.0f };
    static constexpr ParallaxDef kParallaxLvl2_3{ "assets/backgrounds/lvl2/moon_floor.png", 12.0f, {-20.0f, 0.0f}, 200, 60.0f, 1900.0f, {0.72f, 0.72f}, 1900, 1000, 360.0f };
    static constexpr ParallaxDef kParallaxLvl2_4{ "assets/backgrounds/lvl2/moon_front.png", 10.0f, {-1.0f, 0.0f}, 200, 80.0f, 3800.0f, {0.72f, 0.72f}, 3800, 1000, 360.0f };
    static constexpr ParallaxDef kParallaxLvl2_5{ "assets/backgrounds/lvl2/moon_mid.png", 10.0f, {-1.0f, 0.0f}, 200, 80.0f, 3800.0f, {0.72f, 0.72f}, 3800, 1000, 360.0f };
    static constexpr ParallaxDef kParallaxLvl2_6{ "assets/backgrounds/lvl2/moon_sky.png", 10.0f, {-15.0f, 0.0f}, 200, 50.0f, 1280.0f, {2.0f, 2.0f}, 1280, 720, 0.0f };
    static constexpr ParallaxDef kParallaxLvl3_1{ "assets/backgrounds/lvl3/Sky_back_mountain.png", 12.0f, {-20.0f, 0.0f}, 255, 60.0f, 1900.0f, {0.72f, 0.72f}, 1900, 1000, 360.0f };
    static constexpr ParallaxDef kParallaxLvl3_2{ "assets/backgrounds/lvl3/sky_cloud_floor_2.png", 12.0f, {-20.0f, 0.0f}, 255, 60.0f, 1900.0f, {0.72f, 0.72f}, 1900, 1000, 360.0f };
    static constexpr ParallaxDef kParallaxLvl3_3{ "assets/backgrounds/lvl3/sky_cloud_floor.png", 12.0f, {-20.0f, 0.0f}, 255, 60.0f, 1900.0f, {0.72f, 0.72f}, 1900, 1000, 360.0f };
    static constexpr ParallaxDef kParallaxLvl3_4{ "assets/backgrounds/lvl3/Sky_cloud_single.png", 12.0f, {-20.0f, 0.0f}, 255, 60.0f, 1900.0f, {0.72f, 0.72f}, 1900, 1000, 360.0f };
    static constexpr ParallaxDef kParallaxLvl3_5{ "assets/backgrounds/lvl3/sky_clouds.png", 12.0f, {-20.0f, 0.0f}, 255, 60.0f, 1900.0f, {0.72f, 0.72f}, 1900, 1000, 360.0f };
    static constexpr ParallaxDef kParallaxLvl3_6{ "assets/backgrounds/lvl3/Sky_front_cloud.png", 12.0f, {-20.0f, 0.0f}, 255, 60.0f, 1900.0f, {0.72f, 0.72f}, 1900, 1000, 360.0f };
    static constexpr ParallaxDef kParallaxLvl3_7{ "assets/backgrounds/lvl3/sky_front_mountain.png", 12.0f, {-20.0f, 0.0f}, 255, 60.0f, 1900.0f, {0.72f, 0.72f}, 1900, 1000, 360.0f };
    static constexpr ParallaxDef kParallaxLvl3_8{ "assets/backgrounds/lvl3/sky_moon.png", 12.0f, {-15.0f, 0.0f}, 255, 55.0f, 3800.0f, {0.72f, 0.72f}, 3800, 2400, 360.0f };
    static constexpr ParallaxDef kParallaxLvl3_9{ "assets/backgrounds/lvl3/Sky_sky.png", 12.0f, {-20.0f, 0.0f}, 255, 60.0f, 1900.0f, {0.72f, 0.72f}, 1900, 1000, 360.0f };
    static constexpr ParallaxDef kParallaxLvl4_1{ "assets/backgrounds/lvl4/desert_cloud.png", 15.0f, {-30.0f, 0.0f}, 255, 80.0f, 1900.0f, {0.72f, 0.72f}, 1900, 1000, 360.0f };
    static constexpr ParallaxDef kParallaxLvl4_2{ "assets/backgrounds/lvl4/desert_dunefrontt.png", 25.0f, {-30.0f, 0.0f}, 255, 150.0f, 3800.0f, {0.72f, 0.72f}, 3800, 1000, 360.0f };
    static constexpr ParallaxDef kParallaxLvl4_3{ "assets/backgrounds/lvl4/desert_dunemid.png", 12.0f, {-20.0f, 0.0f}, 255, 60.0f, 1900.0f, {0.72f, 0.72f}, 1900, 1000, 360.0f };
    static constexpr ParallaxDef kParallaxLvl4_4{ "assets/backgrounds/lvl4/desert_moon.png", 18.0f, {-40.0f, 0.0f}, 255, 90.0f, 3800.0f, {0.72f, 0.72f}, 3800, 2400, 360.0f };
    static constexpr ParallaxDef kParallaxLvl4_5{ "assets/backgrounds/lvl4/desert_mountain.png", 20.0f, {-25.0f, 0.0f}, 255, 70.0f, 3800.0f, {0.72f, 0.72f}, 3800, 1000, 360.0f };
    static constexpr ParallaxDef kParallaxLvl4_6{ "assets/backgrounds/lvl4/desert_sky.png", 10.0f, {-15.0f, 0.0f}, 255, 50.0f, 1900.0f, {0.72f, 0.72f}, 1900, 1000, 360.0f };

        static constexpr SpriteAnimDef k_rt1_1{
            "player_ship",     "assets/sprites/r-typesheet1.gif",
            101,         3,
            33,          14,
            5,           255,         255, 255,  // RGB tint (white by default)
            true,        33,
            14,          101,
            3,           5,
            2.0f,        {-1.0f, 0.0f},
            {1.0f,  1.0f}
        };
        static constexpr SpriteAnimDef k_rt1_2{
            "shot_insane",     "assets/sprites/r-typesheet1.gif",
            134,         18,
            33,          32,
            5,           255,         255, 255,  // RGB tint (white by default)
            true,        33,
            32,          134,
            18,          4,
            2.0f,        {-1.0f, 0.0f},
            {1.0f,  1.0f}
        };
        static constexpr SpriteAnimDef k_rt1_3{
            "effect_1",     "assets/sprites/r-typesheet1.gif",
            2,           51,
            33,          32,
            5,           255,         255, 255,  // RGB tint (white by default)
            true,        33,
            32,          2,
            51,          8,
            2.0f,        {-1.0f, 0.0f},
            {1.0f,  1.0f}
        };
        static constexpr SpriteAnimDef k_rt1_4{
            "shot_1",     "assets/sprites/r-typesheet1.gif",
            215,         85,
            18,          12,
            5,           255,         255, 255,  // RGB tint (white by default)
            true,        18,
            12,          215,
            85,          3,
            2.0f,        {-1.0f, 0.0f},
            {1.0f,  1.0f}
        };
        static constexpr SpriteAnimDef k_rt1_5{
            "shot_2",     "assets/sprites/r-typesheet1.gif",
            232,         103,
            17,          12,
            5,           255,         255, 255,  // RGB tint (white by default)
            true,        17,
            12,          232,
            103,         2,
            2.0f,        {-1.0f, 0.0f},
            {1.0f,  1.0f}
        };
        static constexpr SpriteAnimDef k_rt1_6{
            "shot_3",     "assets/sprites/r-typesheet1.gif",
            200,         121,
            33,          10,
            5,           255,         255, 255,  // RGB tint (white by default)
            true,        33,
            10,          200,
            121,         2,
            2.0f,        {-1.0f, 0.0f},
            {1.0f,  1.0f}
        };
        static constexpr SpriteAnimDef k_rt1_7{
            "shot_4",     "assets/sprites/r-typesheet1.gif",
            168,         137,
            49,          12,
            5,           255,         255, 255,  // RGB tint (white by default)
            true,        49,
            12,          168,
            137,         2,
            2.0f,        {-1.0f, 0.0f},
            {1.0f,  1.0f}
        };
        static constexpr SpriteAnimDef k_rt1_8{
            "shot_5",     "assets/sprites/r-typesheet1.gif",
            104,         171,
            81,          14,
            5,           255,         255, 255,  // RGB tint (white by default)
            true,        81,
            14,          104,
            171,         2,
            2.0f,        {-1.0f, 0.0f},
            {1.0f,  1.0f}
        };
        static constexpr SpriteAnimDef k_rt1_12{
            "effect_2",    "assets/sprites/r-typesheet1.gif",
            211,         276,
            16,          12,
            5,           255,         255, 255,  // RGB tint (white by default)
            true,        16,
            12,          211,
            276,         4,
            2.0f,        {-1.0f, 0.0f},
            {1.0f,  1.0f}
        };
        static constexpr SpriteAnimDef k_rt1_13{
            "effect_3",    "assets/sprites/r-typesheet1.gif",
            72,          296,
            37,          30,
            5,           255,         255, 255,  // RGB tint (white by default)
            true,        37,
            30,          72,
            296,         6,
            2.0f,        {-1.0f, 0.0f},
            {1.0f,  1.0f}
        };

        static constexpr SpriteAnimDef k_rt2_2{
            "enemy_1",     "assets/sprites/r-typesheet2.gif",
            159,         35,
            24,          16,
            5,           255,         255, 255,  // RGB tint (white by default)
            true,        24,
            16,          159,
            35,          6,
            2.0f,        {-1.0f, 0.0f},
            {1.0f,  1.0f}
        };
        static constexpr SpriteAnimDef k_rt2_3{
            "shot_6",     "assets/sprites/r-typesheet2.gif",
            300,         58,
            18,          6,
            5,           255,         255, 255,  // RGB tint (white by default)
            false,       0,
            0,           0,
            0,           0,
            50.0f,       {-1.0f, 0.0f},
            {1.0f,  1.0f}
        };
        static constexpr SpriteAnimDef k_rt2_4{
            "enemy_2",     "assets/sprites/r-typesheet2.gif",
            300,         71,
            30,          18,
            5,           255,         255, 255,  // RGB tint (white by default)
            true,        30,
            18,          300,
            71,          6,
            2.0f,        {-1.0f, 0.0f},
            {1.0f,  1.0f}
        };
        static constexpr SpriteAnimDef k_rt2_5{
            "shot_7",     "assets/sprites/r-typesheet2.gif",
            266,         94,
            17,          10,
            5,           255,         255, 255,  // RGB tint (white by default)
            true,        17,
            10,          266,
            94,          2,
            2.0f,        {-1.0f, 0.0f},
            {1.0f,  1.0f}
        };
        static constexpr SpriteAnimDef k_rt2_6{
            "effect_4",     "assets/sprites/r-typesheet2.gif",
            101,         118,
            17,          14,
            5,           255,         255, 255,  // RGB tint (white by default)
            true,        17,
            14,          101,
            118,         4,
            2.0f,        {-1.0f, 0.0f},
            {1.0f,  1.0f}
        };
        static constexpr SpriteAnimDef k_rt2_7{
            "effect_5",     "assets/sprites/r-typesheet2.gif",
            157,         316,
            18,          14,
            5,           255,         255, 255,  // RGB tint (white by default)
            true,        18,
            14,          157,
            316,         8,
            2.0f,        {-1.0f, 0.0f},
            {1.0f,  1.0f}
        };
        // New enemy sprite from r-typesheet23.gif (first row, 8 frames)
        static constexpr SpriteAnimDef k_rt23_1{
            "enemy_3",     "assets/sprites/r-typesheet23.gif",
            0,           0,
            33,          33,
            5,           255,         255, 255,
            true,        33,
            33,          0,
            0,           8,
            2.0f,        {-1.0f, 0.0f},
            {1.0f,  1.0f}
        };
        // New enemy sprite from r-typesheet5.gif (single row, first 8 frames)
        static constexpr SpriteAnimDef k_rt5_1{
            "enemy_4",     "assets/sprites/r-typesheet5.gif",
            0,           0,
            66,          36,
            5,           255,         255, 255,
            true,        66,
            36,          0,
            0,           8,
            2.0f,        {-1.0f, 0.0f},
            {1.0f,  1.0f}
        };
        static constexpr SpriteAnimDef k_rt3_1{
            "power_up_heal",     "assets/sprites/r-typesheet3.gif",
            0,           0,           // Red powerup (Heal) - frame 0
            16,          16,          // Each frame is 16x16
            5,           255,         255, 100,  // zIndex 5, RED tint (R=255, G=255, B=100)
            true,        16,          // withAnimation, frameWidth 16
            16,          0,           // frameHeight 16, frameLeft 0
            0,           12,          // frameTop 0, totalFrames 12
            2.0f,        {0.0f, 0.0f}, // speed 2.0, no movement
            {2.0f,  2.0f}             // scale 2x for visibility
        };
        static constexpr SpriteAnimDef k_rt3_2{
            "power_up_double",   "assets/sprites/r-typesheet3.gif",
            64,          0,           // Cyan powerup (DoubleFire) - frame 4 (16*4=64)
            16,          16,          // Each frame is 16x16
            5,           100,         200, 255,  // zIndex 5, CYAN tint (R=100, G=200, B=255)
            true,        16,          // withAnimation, frameWidth 16
            16,          64,          // frameHeight 16, frameLeft 64
            0,           12,          // frameTop 0, totalFrames 12
            2.0f,        {0.0f, 0.0f}, // speed 2.0, no movement
            {2.0f,  2.0f}             // scale 2x for visibility
        };
        static constexpr SpriteAnimDef k_rt3_3{
            "power_up_shield",   "assets/sprites/r-typesheet3.gif",
            128,         0,           // Green powerup (Shield) - frame 8 (16*8=128)
            16,          16,          // Each frame is 16x16
            5,           100,         255, 100,  // zIndex 5, GREEN tint (R=100, G=255, B=100)
            true,        16,          // withAnimation, frameWidth 16
            16,          128,         // frameHeight 16, frameLeft 128
            0,           12,          // frameTop 0, totalFrames 12
            2.0f,        {0.0f, 0.0f}, // speed 2.0, no movement
            {2.0f,  2.0f}             // scale 2x for visibility
        };
        static constexpr SpriteAnimDef k_rt39_1{
            "boss_ship",    "assets/sprites/r-typesheet39.gif",
            33,          1,
            66,         66,
            5,          255,         255, 255,  // RGB tint (white by default)
            false,       66,
            33,         66,
            1,          0,
            1.0f,        {-1.0f, 0.0f},
            {3.0f,  3.0f}
        };
        static constexpr SpriteAnimDef k_rt39_2{
            "boss_shield",    "assets/sprites/r-typesheet39.gif",
            0,          72,
            30,         33,
            5,          255,         255, 255,  // RGB tint (white by default)
            true,       33,
            30,         0,
            72,          4,
            1.0f,        {-1.0f, 0.0f},
            {3.0f,  3.0f}
        };
        // Kraken Alien Boss (r-typesheet35.gif) - 528x574 spritesheet, 2 cols x 4 rows = 8 frames
        // Each frame: 264x143 (approx)
        static constexpr SpriteAnimDef k_rt17_boss{
            "boss3_invincible",    "assets/sprites/r-typesheet17.gif",
            0,          0,
            66,         66,
            5,          200,         200, 255,  // Slight blue tint
            true,       66,
            66,         0,
            0,          8,
            0.8f,       {-1.0f, 0.0f},
            {3.0f,  3.0f} // Increased scale for bigger boss
        };
        static constexpr SpriteAnimDef k_rt35_boss2{
            "boss2_kraken",    "assets/sprites/r-typesheet35.gif",
            0,          0,
            264,        143,
            5,          255,         255, 255,  // White tint
            true,       264,
            143,        0,
            0,          8,
            1.0f,       {-1.0f, 0.0f},
            {2.0f,  2.0f}
        };
        // Kraken Boomerang projectile (uses a menacing tentacle-like look)
        static constexpr SpriteAnimDef k_rt35_boss2_bullet{
            "boss2_bullet",   "assets/sprites/r-typesheet1.gif",
            215,        85,
            18,         12,
            5,          255,         100, 100,  // Red tint for enemy projectile
            true,       18,
            12,         215,
            85,         3,
            5.0f,       {-1.0f, 0.0f},
            {2.0f,  2.0f}
        };
        // Invincible King Boss (r-typesheet17.gif)
        static constexpr SpriteAnimDef k_rt17_king{
            "boss3_king",    "assets/sprites/r-typesheet17.gif",
            0,          0,
            128,        128,
            5,          200,         200, 255,  // Slight blue tint
            true,       128,
            128,        0,
            0,          6,
            0.8f,       {-1.0f, 0.0f},
            {3.0f,  3.0f}
        };

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

        static EntityTemplate player_ship(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt1_1);
        }

        static EntityTemplate shot_insane(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt1_2);
        }

        static EntityTemplate effect_1(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt1_3);
        }

        static EntityTemplate shot_1(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt1_4);
        }

        static EntityTemplate shot_2(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt1_5);
        }

        static EntityTemplate shot_3(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt1_6);
        }

        static EntityTemplate shot_4(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt1_7);
        }

        static EntityTemplate shot_5(const Vec2f &initialPos)
        {
            auto t = makeFromDef(initialPos, k_rt1_8);
            // Default shot_5 sprites point left; flip horizontally so tip faces right
            t.scale.x = -std::abs(t.scale.x);
            return t;
        }

        static EntityTemplate effect_2(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt1_12);
        }

        static EntityTemplate effect_3(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt1_13);
        }

        static EntityTemplate enemy_1(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt2_2);
        }

        static EntityTemplate shot_6(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt2_3);
        }

        static EntityTemplate enemy_2(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt2_4);
        }

        static EntityTemplate enemy_3(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt23_1);
        }

        static EntityTemplate enemy_4(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt5_1);
        }

        static EntityTemplate shot_7(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt2_5);
        }

        static EntityTemplate effect_4(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt2_6);
        }

        static EntityTemplate effect_5(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt2_7);
        }

        // Convenience aliases used by Application.cpp
        static EntityTemplate createBulletPlayer(const Vec2f &initialPos)
        {
            auto t = makeFromDef(initialPos, k_rt2_3);
            t.tag = "player_bullet";
            return t;
        }

        static EntityTemplate createBulletEnemy(const Vec2f &initialPos)
        {
            auto t = makeFromDef(initialPos, k_rt2_3);
            t.tag = "enemy_bullet";
            return t;
        }

        static EntityTemplate createBasicScout2(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt2_2);
        }

        static EntityTemplate createPowerUpHeal(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt3_1);
        }

        static EntityTemplate createPowerUpDoubleFire(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt3_2);
        }

        static EntityTemplate createPowerUpShield(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt3_3);
        }

        static EntityTemplate createBossShip(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt39_1);
        }

        static EntityTemplate createBossShield(const Vec2f &initialPos)
        {
            return makeFromDef(initialPos, k_rt39_2);
        }

        static EntityTemplate createBoss2Kraken(const Vec2f &initialPos)
        {
            auto t = makeFromDef(initialPos, k_rt35_boss2);
            t.tag = "boss2_kraken";
            return t;
        }

        static EntityTemplate createBoss2Bullet(const Vec2f &initialPos)
        {
            auto t = makeFromDef(initialPos, k_rt35_boss2_bullet);
            t.tag = "boss2_bullet";
            return t;
        }

        static EntityTemplate createBossKing(const Vec2f &initialPos)
        {
            auto t = makeFromDef(initialPos, k_rt17_king);
            t.tag = "boss_king";
            return t;
        }

        static EntityTemplate createBoss3Invincible(const Vec2f &initialPos)
        {
            // Anchor boss to bottom: set Y so bottom of sprite is at screen bottom (assuming 720p)
            float bossHeight = 66.0f * k_rt17_boss.scale.y;
            float screenHeight = 720.0f;
            Vec2f anchoredPos = {initialPos.x, screenHeight - bossHeight};
            auto t = makeFromDef(anchoredPos, k_rt17_boss);
            t.tag = "boss3_invincible";
            return t;
        }
    };

    /**
     * @class EnemyBuilder
     * @brief Spawns enemy entities from an
     * EnemyTemplate.
     */
    class EntityBuilder {
        public:
            explicit EntityBuilder(ecs::Registry &registry);

            [[nodiscard]]
            auto spawn(const EntityTemplate &t)
                -> std::expected<ecs::Entity, Error>;

            void kill(ecs::Entity entity);

            void update(ecs::Entity entity, const EntityTemplate &t);

        private:
            ecs::Registry &_registry;
    };

}
