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
    auto result = registry->spawn();
    ASSERT_TRUE(result.has_value());
}

TEST_F(RegistryTest, KillEntity) {
    auto entity = registry->spawn();
    ASSERT_TRUE(entity.has_value());
    ASSERT_NO_THROW(registry->kill(entity.value()));
}

TEST_F(RegistryTest, RegisterComponent) {
    auto result = registry->registerComponent<Transform>();
    ASSERT_TRUE(result.has_value());
}

TEST_F(RegistryTest, add) {
    auto regResult = registry->registerComponent<Transform>();
    ASSERT_TRUE(regResult.has_value());
    
    auto entity = registry->spawn();
    ASSERT_TRUE(entity.has_value());
    
    auto result = registry->add<Transform>(entity.value());
    ASSERT_TRUE(result.has_value());
}

TEST_F(RegistryTest, GetComponentsNotRegisteredReturnsError) {
    auto res = registry->getComponents<Velocity>();
    ASSERT_FALSE(res.has_value());
    EXPECT_EQ(res.error().code(), rtp::ErrorCode::ComponentMissing);
}

TEST_F(RegistryTest, RegisterComponentIsIdempotent) {
    auto first = registry->registerComponent<Health>();
    ASSERT_TRUE(first.has_value());
    auto second = registry->registerComponent<Health>();
    EXPECT_FALSE(second.has_value());
    EXPECT_EQ(second.error().code(), rtp::ErrorCode::InternalRuntimeError);
}

TEST_F(RegistryTest, ClearEmptiesArraysAndAllowsRespawn) {
    auto tsResult = registry->registerComponent<Transform>();
    ASSERT_TRUE(tsResult.has_value());
    auto &ts = tsResult->get();
    auto e1 = registry->spawn();
    ASSERT_TRUE(e1.has_value());
    ts.emplace(e1.value(), Transform{});

    EXPECT_EQ(ts.size(), 1u);
    registry->clear();
    EXPECT_EQ(ts.size(), 0u);

    auto e2 = registry->spawn();
    ASSERT_TRUE(e2.has_value());
}

TEST_F(RegistryTest, ZipViewThreeComponentsAlign) {
    auto tsResult = registry->registerComponent<Transform>();
    auto vsResult = registry->registerComponent<Velocity>();
    auto hsResult = registry->registerComponent<Health>();
    ASSERT_TRUE(tsResult.has_value());
    ASSERT_TRUE(vsResult.has_value());
    ASSERT_TRUE(hsResult.has_value());
    auto &ts = tsResult->get();
    auto &vs = vsResult->get();
    auto &hs = hsResult->get();

    auto e1 = registry->spawn(); // has T,V,H
    auto e2 = registry->spawn(); // has T only
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
