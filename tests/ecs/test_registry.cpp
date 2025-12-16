#include <gtest/gtest.h>
#include "RType/ECS/Registry.hpp"
#include "RType/ECS/Components/Transform.hpp"
#include "RType/ECS/Components/Velocity.hpp"
#include "RType/ECS/Components/Health.hpp"

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

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
