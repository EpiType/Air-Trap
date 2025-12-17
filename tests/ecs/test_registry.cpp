#include <gtest/gtest.h>
#include "RType/ECS/Registry.hpp"
#include "RType/ECS/Components/Transform.hpp"
#include "RType/ECS/Components/Velocity.hpp"
#include "RType/ECS/Components/Health.hpp"
#include "RType/Error.hpp"

using namespace rtp::ecs;
using namespace rtp::ecs::components;

class RegistryTest : public ::testing::Test {
protected:
    void SetUp() override {
        registry = std::make_unique<Registry>();
    }

    std::unique_ptr<Registry> registry;
};

TEST_F(RegistryTest, SpawnEntity) {
    auto result = registry->spawnEntity();
    ASSERT_TRUE(result.has_value());
}

TEST_F(RegistryTest, KillEntity) {
    auto entity = registry->spawnEntity();
    ASSERT_TRUE(entity.has_value());
    ASSERT_NO_THROW(registry->killEntity(entity.value()));
}

TEST_F(RegistryTest, RegisterComponent) {
    ASSERT_NO_THROW(registry->registerComponent<Transform>());
}

TEST_F(RegistryTest, AddComponent) {
    registry->registerComponent<Transform>();
    
    auto entity = registry->spawnEntity();
    ASSERT_TRUE(entity.has_value());
    
    auto result = registry->addComponent<Transform>(entity.value());
    ASSERT_TRUE(result.has_value());
}

TEST_F(RegistryTest, GetComponentsNotRegisteredReturnsError) {
    auto res = registry->getComponents<Velocity>();
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error().code(), rtp::ErrorCode::ComponentMissing);
}

TEST_F(RegistryTest, RegisterComponentIsIdempotent) {
    auto &a = registry->registerComponent<Health>();
    auto &b = registry->registerComponent<Health>();
    EXPECT_EQ(&a, &b);
}

TEST_F(RegistryTest, ClearEmptiesArraysAndAllowsRespawn) {
    auto &ts = registry->registerComponent<Transform>();
    auto e1 = registry->spawnEntity();
    ASSERT_TRUE(e1.has_value());
    ts.emplace(e1.value(), Transform{});

    EXPECT_EQ(ts.size(), 1u);
    registry->clear();
    EXPECT_EQ(ts.size(), 0u);

    auto e2 = registry->spawnEntity();
    ASSERT_TRUE(e2.has_value());
}

TEST_F(RegistryTest, ZipViewThreeComponentsAlign) {
    auto &ts = registry->registerComponent<Transform>();
    auto &vs = registry->registerComponent<Velocity>();
    auto &hs = registry->registerComponent<Health>();

    auto e1 = registry->spawnEntity(); // has T,V,H
    auto e2 = registry->spawnEntity(); // has T only
    ASSERT_TRUE(e1.has_value());
    ASSERT_TRUE(e2.has_value());

    ts.emplace(e1.value(), Transform{});
    vs.emplace(e1.value(), Velocity{});
    hs.emplace(e1.value(), Health{});

    ts.emplace(e2.value(), Transform{});

    std::size_t count = 0;
    for (auto [t, v, h] : registry->zipView<Transform, Velocity, Health>()) {
        (void)t; (void)v; (void)h;
        ++count;
    }
    EXPECT_EQ(count, 1u);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
