#include <gtest/gtest.h>
// #include "ecs/Registry.hpp"
// #include "ecs/components/PositionComponent.hpp"
// #include "ecs/components/VelocityComponent.hpp"

// TODO: Implémenter les tests quand les composants seront créés

// Test de création d'entité
// TEST(RegistryTest, CreateEntity) {
//     ecs::Registry registry;
//     auto entity = registry.createEntity();
//     EXPECT_NE(entity, 0);
// }

// Test d'ajout de composant
// TEST(RegistryTest, AddComponent) {
//     ecs::Registry registry;
//     auto entity = registry.createEntity();
//     registry.addComponent(entity, PositionComponent{100.0f, 200.0f});
//     EXPECT_TRUE(registry.hasComponent<PositionComponent>(entity));
// }

// Test de récupération de composant
// TEST(RegistryTest, GetComponent) {
//     ecs::Registry registry;
//     auto entity = registry.createEntity();
//     registry.addComponent(entity, PositionComponent{100.0f, 200.0f});
//     auto& pos = registry.getComponent<PositionComponent>(entity);
//     EXPECT_FLOAT_EQ(pos.x, 100.0f);
//     EXPECT_FLOAT_EQ(pos.y, 200.0f);
// }

// Test de suppression d'entité
// TEST(RegistryTest, DestroyEntity) {
//     ecs::Registry registry;
//     auto entity = registry.createEntity();
//     registry.addComponent(entity, PositionComponent{100.0f, 200.0f});
//     registry.destroyEntity(entity);
//     EXPECT_FALSE(registry.hasComponent<PositionComponent>(entity));
// }

// Test de vue sur les composants
// TEST(RegistryTest, ViewComponents) {
//     ecs::Registry registry;
//     auto e1 = registry.createEntity();
//     auto e2 = registry.createEntity();
//     registry.addComponent(e1, PositionComponent{0.0f, 0.0f});
//     registry.addComponent(e1, VelocityComponent{1.0f, 1.0f});
//     registry.addComponent(e2, PositionComponent{10.0f, 10.0f});
//     
//     auto view = registry.view<PositionComponent, VelocityComponent>();
//     EXPECT_EQ(view.size(), 1);  // Seulement e1 a les deux composants
// }

// Placeholder test pour que GTest compile
TEST(RegistryTest, Placeholder) {
    EXPECT_TRUE(true);
}
