#include <iostream>
#include "Core/Application.hpp"

int main()
{
    Client::Core::Application app;
    app.run();
    return 0;
}
//#include <SFML/Graphics.hpp>
//
//#include "RType/ECS/Registry.hpp"
//#include "RType/ECS/SystemManager.hpp"
//
///* Components */
//#include "RType/ECS/Components/Transform.hpp"
//#include "RType/ECS/Components/Velocity.hpp"
//#include "RType/ECS/Components/Controllable.hpp"
//#include "RType/ECS/Components/Sprite.hpp"
//#include "RType/ECS/Components/Animation.hpp"
//
///* Systems */
//#include "Systems/AnimationSystem.hpp"
//#include "Systems/InputSystem.hpp"
//#include "Systems/MovementSystem.hpp"
//#include "Systems/RenderSystem.hpp"
//
//#include "RType/Math/Vec2.hpp"
//
//int main() {
//    rtp::ecs::Registry registry;
//    rtp::ecs::SystemManager sysManager(registry);
//    
//    sf::RenderWindow window(sf::VideoMode({800, 600}), "R-Type ECS Client");
//    window.setFramerateLimit(60);
//
//    registry.registerComponent<rtp::ecs::components::Transform>();
//    registry.registerComponent<rtp::ecs::components::Velocity>();
//    registry.registerComponent<rtp::ecs::components::Controllable>();
//    registry.registerComponent<rtp::ecs::components::Sprite>();
//    registry.registerComponent<rtp::ecs::components::Animation>();
//
//    sysManager.addSystem<rtp::client::InputSystem>(registry);
//    sysManager.addSystem<rtp::client::MovementSystem>(registry);
//    sysManager.addSystem<rtp::client::AnimationSystem>(registry);
//    auto renderSys = std::make_unique<rtp::client::RenderSystem>(registry, window);
//
//    auto playerRes = registry.spawnEntity();
//    if (playerRes) {
//        rtp::ecs::Entity p = playerRes.value();
//
//        registry.addComponent<rtp::ecs::components::Transform>(p, 
//            rtp::Vec2f{200.f, 150.f}, 0.0f, rtp::Vec2f{1.0f, 1.0f});
//        
//        registry.addComponent<rtp::ecs::components::Velocity>(p);
//        registry.addComponent<rtp::ecs::components::Controllable>(p);
//
//        rtp::ecs::components::Sprite spriteData;
//        spriteData.texturePath = "assets/sprites/r-typesheet42.gif";
//        spriteData.rectLeft = 0;
//        spriteData.rectTop = 0;
//        spriteData.rectWidth = 33;
//        spriteData.rectHeight = 17;
//        spriteData.zIndex = 10;
//        spriteData.red = 255;
//
//
//        auto drawRes = registry.addComponent<rtp::ecs::components::Sprite>(p, spriteData);
//
//        rtp::ecs::components::Animation animData;
//        animData.frameLeft = 0;
//        animData.frameTop = 0;
//        animData.frameWidth = 33;
//        animData.frameHeight = 17;
//        animData.totalFrames = 5;
//        animData.frameDuration = 0.1f;
//        animData.currentFrame = 0;
//        animData.elapsedTime = 0.0f;
//
//        registry.addComponent<rtp::ecs::components::Animation>(p, animData);
//
//        if (!drawRes) {
//            rtp::log::error("Erreur critique lors de l'ajout du Sprite.");
//        }
//    }
//
//    sf::Clock clock;
//    while (window.isOpen()) {
//        while (const std::optional event = window.pollEvent()) {
//            if (event->is<sf::Event::Closed>())
//                window.close();
//        }
//        rtp::log::info("Frame Start");
//
//        float dt = clock.restart().asSeconds();
//        sysManager.update(dt);
//        renderSys->update(dt);
//    }
// 
//    return 0;
//}