#include <gtest/gtest.h>
#include "RType/ECS/Registry.hpp"
#include "RType/ECS/Components/Transform.hpp"
#include "RType/ECS/Components/Velocity.hpp"

using namespace rtp;
using namespace rtp::ecs;
using namespace rtp::ecs::components;

TEST(ZipViewTest, IterationAlignsOnCommonEntities) {
    Registry reg;
    auto tsResult = reg.subscribe<Transform>();
    auto vsResult = reg.subscribe<Velocity>();
    ASSERT_TRUE(tsResult.has_value());
    ASSERT_TRUE(vsResult.has_value());
    auto &ts = tsResult->get();
    auto &vs = vsResult->get();

    auto e1 = reg.spawn();
    auto e2 = reg.spawn();
    ASSERT_TRUE(e1.has_value());
    ASSERT_TRUE(e2.has_value());

    ts.emplace(e1.value(), Transform{Vec2f{1.f, 1.f}, 0.f, Vec2f{1.f,1.f}});
    ts.emplace(e2.value(), Transform{Vec2f{2.f, 2.f}, 0.f, Vec2f{1.f,1.f}});
    vs.emplace(e2.value(), Velocity{Vec2f{1.f, 0.f}, 3.f});

    size_t count = 0;
    for (auto [t, v] : reg.zipView<Transform, Velocity>()) {
        (void)v;
        ++count;
        EXPECT_FLOAT_EQ(t.position.x, 2.f);
        EXPECT_FLOAT_EQ(t.position.y, 2.f);
    }
    EXPECT_EQ(count, 1u);
}

TEST(ZipViewTest, UpdatesAfterErase) {
    Registry reg;
    auto tsResult = reg.subscribe<Transform>();
    auto vsResult = reg.subscribe<Velocity>();
    ASSERT_TRUE(tsResult.has_value());
    ASSERT_TRUE(vsResult.has_value());
    auto &ts = tsResult->get();
    auto &vs = vsResult->get();

    auto e1 = reg.spawn();
    ASSERT_TRUE(e1.has_value());

    ts.emplace(e1.value(), Transform{Vec2f{5.f, 5.f}, 0.f, Vec2f{1.f,1.f}});
    vs.emplace(e1.value(), Velocity{Vec2f{0.f, 1.f}, 2.f});

    size_t before = 0;
    for (auto &&tuple : reg.zipView<Transform, Velocity>()) { (void)tuple; ++before; }
    EXPECT_EQ(before, 1u);

    vs.erase(e1.value());

    size_t after = 0;
    for (auto &&tuple : reg.zipView<Transform, Velocity>()) { (void)tuple; ++after; }
    EXPECT_EQ(after, 0u);
}
