#include <iostream>
#include <SFML/Graphics.hpp>

#include "RType/ECS/Registry.hpp"
#include "RType/ECS/SystemManager.hpp"

/* Components */
#include "RType/ECS/Components/Transform.hpp"
#include "RType/ECS/Components/Velocity.hpp"
#include "RType/ECS/Components/Controllable.hpp"
#include "RType/ECS/Components/Sprite.hpp"

/* Systems */
#include "Systems/InputSystem.hpp"
#include "Systems/MovementSystem.hpp"
#include "Systems/RenderSystem.hpp"

#include "RType/Math/Vec2.hpp"

int main() {
    rtp::ecs::Registry registry;
    rtp::ecs::SystemManager sysManager(registry);
    
    sf::RenderWindow window(sf::VideoMode({800, 600}), "R-Type ECS Client");
    window.setFramerateLimit(60);

    registry.registerComponent<rtp::ecs::components::Transform>();
    registry.registerComponent<rtp::ecs::components::Velocity>();
    registry.registerComponent<rtp::ecs::components::Controllable>();
    registry.registerComponent<rtp::ecs::components::Sprite>();

    sysManager.addSystem<rtp::client::InputSystem>(registry);
    sysManager.addSystem<rtp::client::MovementSystem>(registry);
    auto renderSys = std::make_unique<rtp::client::RenderSystem>(registry, window);

    auto playerRes = registry.spawnEntity();
    if (playerRes) {
        rtp::ecs::Entity p = playerRes.value();

        registry.addComponent<rtp::ecs::components::Transform>(p, 
            rtp::Vec2f{400.f, 300.f}, 0.0f, rtp::Vec2f{2.0f, 2.0f});
        
        registry.addComponent<rtp::ecs::components::Velocity>(p);
        registry.addComponent<rtp::ecs::components::Controllable>(p);

        rtp::ecs::components::Sprite spriteData;
        spriteData.texturePath = "assets/sprites/players/image.png";
        spriteData.rectLeft = 0;
        spriteData.rectTop = 0;
        spriteData.rectWidth = 33;
        spriteData.rectHeight = 17;
        spriteData.zIndex = 10;
        spriteData.red = 255; 

        auto drawRes = registry.addComponent<rtp::ecs::components::Sprite>(p, spriteData);

        if (!drawRes) {
            rtp::log::error("Erreur critique lors de l'ajout du Sprite.");
        }
    }

    sf::Clock clock;
    while (window.isOpen()) {
        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();
        }
        rtp::log::info("Frame Start");

        float dt = clock.restart().asSeconds();
        sysManager.update(dt);
        renderSys->update(dt);
    }

    return 0;
}