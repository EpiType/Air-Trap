
#include <gtest/gtest.h>

#include "RType/ECS/Registry.hpp"
#include "RType/ECS/Components/Transform.hpp"
#include "RType/ECS/Components/Velocity.hpp"
#include "RType/ECS/Components/Health.hpp"
#include "RType/ECS/Components/Damage.hpp"
#include "RType/ECS/Components/Sprite.hpp"
#include "RType/ECS/Components/BoundingBox.hpp"
#include "RType/ECS/Components/Hitbox.hpp"
#include "RType/ECS/Components/ParallaxLayer.hpp"
#include "RType/ECS/Components/Controllable.hpp"
#include "RType/ECS/Components/InputComponent.hpp"

using namespace rtp;
using namespace rtp::ecs;
using namespace rtp::ecs::components;

// Transform defaults
TEST(ECS_Component_Transform, Defaults) {
    Transform t;
    EXPECT_FLOAT_EQ(t.position.x, 0.0f);
    EXPECT_FLOAT_EQ(t.position.y, 0.0f);
    EXPECT_FLOAT_EQ(t.rotation, 0.0f);
    EXPECT_FLOAT_EQ(t.scale.x, 1.0f);
    EXPECT_FLOAT_EQ(t.scale.y, 1.0f);
}

// Velocity defaults and mutation
TEST(ECS_Component_Velocity, DefaultsAndMutation) {
    Velocity v;
    EXPECT_FLOAT_EQ(v.direction.x, 0.0f);
    EXPECT_FLOAT_EQ(v.direction.y, 0.0f);
    EXPECT_FLOAT_EQ(v.speed, 0.0f);

    v.direction = Vec2f{1.0f, -0.5f};
    v.speed = 2.5f;
    EXPECT_FLOAT_EQ(v.direction.x, 1.0f);
    EXPECT_FLOAT_EQ(v.direction.y, -0.5f);
    EXPECT_FLOAT_EQ(v.speed, 2.5f);
}

// Health defaults
TEST(ECS_Component_Health, Defaults) {
    Health h;
    EXPECT_EQ(h.currentHealth, 100);
    EXPECT_EQ(h.maxHealth, 100);
}

// Damage defaults
TEST(ECS_Component_Damage, Defaults) {
    Damage d;
    EXPECT_EQ(d.amount, 0);
    EXPECT_TRUE(d.sourceEntity.isNull());
}

// Sprite defaults
TEST(ECS_Component_Sprite, Defaults) {
    Sprite s;
    EXPECT_EQ(s.texturePath, "0");
    EXPECT_EQ(s.rectLeft, 0);
    EXPECT_EQ(s.rectTop, 0);
    EXPECT_EQ(s.rectWidth, 0);
    EXPECT_EQ(s.rectHeight, 0);
    EXPECT_EQ(s.zIndex, 0);
    EXPECT_EQ(s.opacity, 255);
    EXPECT_EQ(s.red, 255);
    EXPECT_EQ(s.green, 255);
    EXPECT_EQ(s.blue, 255);
}

// BoundingBox defaults
TEST(ECS_Component_BoundingBox, Defaults) {
    BoundingBox b;
    EXPECT_FLOAT_EQ(b.width, 0.0f);
    EXPECT_FLOAT_EQ(b.height, 0.0f);
}

// Hitbox defaults
TEST(ECS_Component_Hitbox, Defaults) {
    Hitbox h;
    EXPECT_FLOAT_EQ(h.width, 0.0f);
    EXPECT_FLOAT_EQ(h.height, 0.0f);
    EXPECT_FLOAT_EQ(h.radius, 0.0f);
    EXPECT_FLOAT_EQ(h.offsetX, 0.0f);
    EXPECT_FLOAT_EQ(h.offsetY, 0.0f);
    EXPECT_EQ(h.shape, CollisionShape::Rectangle);
    EXPECT_EQ(h.collisionType, CollisionType::Solid);
}

// ParallaxLayer defaults
TEST(ECS_Component_ParallaxLayer, Defaults) {
    ParallaxLayer p;
    EXPECT_FLOAT_EQ(p.textureWidth, 0.0f);
}

// Controllable is a tag component
TEST(ECS_Component_Controllable, TagPresenceViaRegistry) {
    Registry reg;
    auto arrResult = reg.subscribe<Controllable>();
    ASSERT_TRUE(arrResult.has_value());
    auto &arr = arrResult->get();
    auto e = reg.spawn();
    ASSERT_TRUE(e.has_value());
    arr.emplace(e.value());
    EXPECT_TRUE(arr.has(e.value()));
    EXPECT_EQ(arr.size(), 1u);
}

// Server-side InputComponent defaults and bitmask
TEST(ECS_Component_InputComponent, DefaultsAndBits) {
    components::server::InputComponent input;
    EXPECT_EQ(input.mask, 0);
    EXPECT_EQ(input.lastProcessedTick, 0u);

    input.mask |= components::server::InputComponent::MoveUp;
    input.mask |= components::server::InputComponent::MoveLeft;
    EXPECT_NE(input.mask & components::server::InputComponent::MoveUp, 0);
    EXPECT_NE(input.mask & components::server::InputComponent::MoveLeft, 0);
    EXPECT_EQ(input.mask & components::server::InputComponent::MoveDown, 0);
}

// Registry integration: add/get/overwrite/erase for Health
TEST(ECS_Component_RegistryIntegration, AddOverwriteErase_Health) {
    Registry reg;
    auto healthsResult = reg.subscribe<Health>();
    ASSERT_TRUE(healthsResult.has_value());
    auto &healths = healthsResult->get();
    auto e = reg.spawn();
    ASSERT_TRUE(e.has_value());

    // Add custom health
    healths.emplace(e.value(), Health{50, 100});
    ASSERT_TRUE(healths.has(e.value()));
    EXPECT_EQ(healths[e.value()].currentHealth, 50);
    EXPECT_EQ(healths.size(), 1u);

    // Overwrite with emplace on same entity
    healths.emplace(e.value(), Health{30, 100});
    EXPECT_EQ(healths[e.value()].currentHealth, 30);
    EXPECT_EQ(healths.size(), 1u); // still one component stored

    // Erase and check absence
    healths.erase(e.value());
    EXPECT_FALSE(healths.has(e.value()));
    EXPECT_EQ(healths.size(), 0u);
}

// Registry view order and values for Transform
TEST(ECS_Component_RegistryView, TransformViewOrder) {
    Registry reg;
    auto transformsResult = reg.subscribe<Transform>();
    ASSERT_TRUE(transformsResult.has_value());
    auto &transforms = transformsResult->get();

    auto e1 = reg.spawn();
    auto e2 = reg.spawn();
    ASSERT_TRUE(e1.has_value());
    ASSERT_TRUE(e2.has_value());

    transforms.emplace(e1.value(), Transform{Vec2f{1.f, 2.f}, 15.f, Vec2f{1.f,1.f}});
    transforms.emplace(e2.value(), Transform{Vec2f{3.f, 4.f}, 30.f, Vec2f{2.f,2.f}});

    auto span = reg.view<Transform>();
    ASSERT_EQ(span.size(), 2u);
    EXPECT_FLOAT_EQ(span[0].position.x, 1.f);
    EXPECT_FLOAT_EQ(span[0].position.y, 2.f);
    EXPECT_FLOAT_EQ(span[1].position.x, 3.f);
    EXPECT_FLOAT_EQ(span[1].position.y, 4.f);
}
