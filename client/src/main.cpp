/*
** EPITECH PROJECT, 2025
** Air-Trap Client Prototype
** File description:
** main.cpp - ECS & SFML 3 Integration Test
*/

#include <SFML/Graphics.hpp>
#include <iostream>
#include <random>
#include <vector>

// --- Imports de ton Core ECS ---
#include "RType/ECS/Coordinator.hpp"
#include "RType/ECS/System/System.hpp"

// ==========================================
// 1. DÉFINITION DES COMPOSANTS (DATA)
// (Normalement dans common/include/RType/ECS/Components/...)
// ==========================================

struct Position {
    float x;
    float y;
};

struct Velocity {
    float dx;
    float dy;
};

// Un composant visuel simple pour le test (pas besoin de texture)
struct ColoredRect {
    float width;
    float height;
    uint8_t r, g, b, a;
};

// ==========================================
// 2. DÉFINITION DES SYSTÈMES (LOGIQUE)
// (Normalement dans client/src/Systems/...)
// ==========================================

// Système qui fait bouger les carrés et rebondir sur les bords
class PhysicsSystem : public rtp::ecs::System {
public:
    void update(rtp::ecs::Coordinator& g, float dt, int winWidth, int winHeight) {
        for (auto const& entity : _Entities) {
            // Récupération des composants (si tu utilises std::expected, gère le !)
            // Ici je suppose un accès direct pour le proto
            auto& pos = g.getComponent<Position>(entity);
            auto& vel = g.getComponent<Velocity>(entity);

            // Mouvement
            pos.x += vel.dx * dt;
            pos.y += vel.dy * dt;

            // Rebondir sur les murs (Bouncing)
            if (pos.x <= 0 || pos.x >= winWidth - 20) vel.dx = -vel.dx;
            if (pos.y <= 0 || pos.y >= winHeight - 20) vel.dy = -vel.dy;
        }
    }
};

// Système qui dessine les carrés SFML
class RenderSystem : public rtp::ecs::System {
public:
    void update(rtp::ecs::Coordinator& g, sf::RenderWindow& window) {
        // On utilise un seul rectangle "Flyweight" pour dessiner tout le monde
        static sf::RectangleShape rectShape;

        for (auto const& entity : _Entities) {
            auto& pos = g.getComponent<Position>(entity);
            auto& color = g.getComponent<ColoredRect>(entity);

            // Configuration du rectangle
            rectShape.setSize({color.width, color.height});
            rectShape.setPosition({pos.x, pos.y});
            rectShape.setFillColor(sf::Color(color.r, color.g, color.b, color.a));

            // Draw
            window.draw(rectShape);
        }
    }
};

// ==========================================
// 3. MAIN (INITIALISATION & BOUCLE)
// ==========================================

int main() {
    // A. Init SFML 3
    const int WIDTH = 1280;
    const int HEIGHT = 720;
    sf::RenderWindow window(sf::VideoMode({(unsigned int)WIDTH, (unsigned int)HEIGHT}), "Air-Trap ECS Test");
    window.setFramerateLimit(60);

    // B. Init ECS Coordinator
    rtp::ecs::Coordinator gCoordinator;
    gCoordinator.init();

    // C. Register Components
    gCoordinator.registerComponent<Position>();
    gCoordinator.registerComponent<Velocity>();
    gCoordinator.registerComponent<ColoredRect>();

    // D. Register Systems
    // 1. Physics
    auto physicsSystem = gCoordinator.registerSystem<PhysicsSystem>();
    rtp::ecs::Signature signaturePhysics;
    signaturePhysics.set(gCoordinator.getComponentType<Position>());
    signaturePhysics.set(gCoordinator.getComponentType<Velocity>());
    gCoordinator.setSystemSignature<PhysicsSystem>(signaturePhysics);

    // 2. Render
    auto renderSystem = gCoordinator.registerSystem<RenderSystem>();
    rtp::ecs::Signature signatureRender;
    signatureRender.set(gCoordinator.getComponentType<Position>());
    signatureRender.set(gCoordinator.getComponentType<ColoredRect>());
    gCoordinator.setSystemSignature<RenderSystem>(signatureRender);

    // E. Création des Entités (500 carrés volants !)
    std::mt19937 rng(std::random_device{}());
    std::uniform_real_distribution<float> distX(0, WIDTH);
    std::uniform_real_distribution<float> distY(0, HEIGHT);
    std::uniform_real_distribution<float> distVel(-200, 200);
    std::uniform_int_distribution<int> distColor(50, 255);

    for (int i = 0; i < 500; ++i) {
        auto entity = gCoordinator.createEntity();

        gCoordinator.addComponent(entity, Position{distX(rng), distY(rng)});
        gCoordinator.addComponent(entity, Velocity{distVel(rng), distVel(rng)});

        uint8_t r = distColor(rng);
        uint8_t g = distColor(rng);
        uint8_t b = distColor(rng);
        gCoordinator.addComponent(entity, ColoredRect{20.0f, 20.0f, r, g, b, 255});
    }

    std::cout << "✅ ECS Initialized with 500 entities." << std::endl;

    // F. Game Loop
    sf::Clock clock;
    while (window.isOpen()) {
        float dt = clock.restart().asSeconds();

        // 1. Events (SFML 3 style)
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPressed->code == sf::Keyboard::Key::Escape)
                    window.close();
            }
        }

        // 2. Update Logic (ECS)
        physicsSystem->update(gCoordinator, dt, WIDTH, HEIGHT);

        // 3. Render (ECS + SFML)
        window.clear(sf::Color(20, 20, 30)); // Fond gris foncé
        renderSystem->update(gCoordinator, window);
        window.display();
    }

    return 0;
}