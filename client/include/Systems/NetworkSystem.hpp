/**
 * File   : NetworkSystem.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/12/2025
 */

#ifndef RTYPE_NETWORK_SYSTEM_HPP_
    #define RTYPE_NETWORK_SYSTEM_HPP_

    #include "RType/ECS/Registry.hpp"
    #include "RType/Logger.hpp"
    #include "RType/ECS/ISystem.hpp"
    #include "Network/ClientNetwork.hpp"
    
    // Components
    #include "RType/ECS/Components/Transform.hpp"
    #include "RType/ECS/Components/Sprite.hpp"
    #include "RType/ECS/Components/Controllable.hpp"
    #include "RType/ECS/Components/Velocity.hpp"

    #include "RType/Network/Packet.hpp"
    #include <unordered_map>
    #include <SFML/Window/Keyboard.hpp>

namespace rtp::client {

    class NetworkSystem : public rtp::ecs::ISystem {
        public:
            NetworkSystem(rtp::ecs::Registry& r, rtp::client::ClientNetwork& network) 
                : _r(r), _network(network) 
            {
                _network.start();
                exit(0);
            };

            void update(float dt) override {
                this->sendPlayerInput();
                this->processNetworkEvents();
            }
            
        private:
            /**
             * @brief Send player input to the server
             * @note Scans entities with 'Controllable' tag and reads SFML Keyboard state
             */
            void sendPlayerInput() {
                try {
                    // On cherche les entités qui ont le tag Controllable
                    auto view = _r.zipView<rtp::ecs::components::Controllable>();

                    for (auto&& [controllable] : view) {
                        rtp::net::Packet packet(rtp::net::OpCode::InputTick);
                        rtp::net::InputPayload payload;
                        
                        // Construction du masque de bits (Bitmask)
                        // Bit 0: UP, 1: DOWN, 2: LEFT, 3: RIGHT, 4: SHOOT
                        payload.inputMask = 0;
                        
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Up) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Z))
                            payload.inputMask |= (1 << 0);
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Down) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::S))
                            payload.inputMask |= (1 << 1);
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Left) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Q))
                            payload.inputMask |= (1 << 2);
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Right) || sf::Keyboard::isKeyPressed(sf::Keyboard::Key::D))
                            payload.inputMask |= (1 << 3);
                        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
                            payload.inputMask |= (1 << 4);

                        packet << payload;

                        // Envoi en UDP (non-bloquant, rapide)
                        _network.sendPacket(packet, rtp::net::NetworkMode::UDP);
                    }
                } catch (const std::exception& e) {
                    // zipView peut throw si le composant n'est pas enregistré
                }
            };

            /**
             * @brief Process incoming network events
             */
            void processNetworkEvents() {
                while (!_network.hasPendingPackets().empty()) {
                    rtp::net::Packet packet = _network.popPacket();

                    switch (packet.header.opCode) {
                        case rtp::net::OpCode::EntitySnapshot: {
                            rtp::net::EntitySnapshotPayload payload;
                            packet >> payload;
                            updateLocalEntity(payload);
                            break;
                        }
                        case rtp::net::OpCode::EntitySpawn: {
                            rtp::net::EntitySpawnPayload payload;
                            packet >> payload;
                            handleEntitySpawn(payload);
                            break;
                        }
                        case rtp::net::OpCode::EntityDeath: {
                            rtp::net::EntityDeathPayload payload;
                            packet >> payload;
                            handleEntityDeath(payload);
                            break;
                        }
                        default:
                            break;
                    }
                }
            };

            // --- GESTION SPAWN ---
            void handleEntitySpawn(const rtp::net::EntitySpawnPayload& payload) {
                if (_serverToClientEntity.find(payload.netId) != _serverToClientEntity.end()) return;

                auto newEntityRes = _r.spawnEntity();
                if (!newEntityRes) return;
                
                rtp::ecs::Entity newEntity = newEntityRes.value();
                _serverToClientEntity[payload.netId] = newEntity;

                // Ajout Transform
                _r.addComponent<rtp::ecs::components::Transform>(newEntity, 
                    payload.position, 
                    0.0f, 
                    rtp::Vec2f{1.0f, 1.0f});

                // Ajout Velocity (nécessaire pour la physique même si snapshot)
                _r.addComponent<rtp::ecs::components::Velocity>(newEntity);

                // Configuration Sprite selon le type
                configureSprite(newEntity, payload.type);

                rtp::log::info("Network Spawn: NetID {} Type {}", payload.netId, (int)payload.type);
            }

            // --- GESTION DEATH ---
            void handleEntityDeath(const rtp::net::EntityDeathPayload& payload) {
                auto it = _serverToClientEntity.find(payload.netId);
                if (it != _serverToClientEntity.end()) {
                    _r.killEntity(it->second);
                    _serverToClientEntity.erase(it);
                    rtp::log::info("Network Death: NetID {}", payload.netId);
                }
            }

            // --- GESTION SNAPSHOT (Update) ---
            void updateLocalEntity(const rtp::net::EntitySnapshotPayload& snapshot) {
                auto it = _serverToClientEntity.find(snapshot.netId);

                if (it != _serverToClientEntity.end()) {
                    rtp::ecs::Entity localEntity = it->second;

                    // Mise à jour via getComponents pour la performance
                    auto transformRes = _r.getComponents<rtp::ecs::components::Transform>();
                    
                    if (transformRes) {
                        auto& transformArray = transformRes.value().get();

                        if (transformArray.has(localEntity)) {
                            auto& transform = transformArray[localEntity];
                            transform.position = snapshot.position;
                            transform.rotation = static_cast<float>(snapshot.angle);
                        }
                    }
                } else {
                    // Lazy Spawn : On a reçu un mouvement pour une entité inconnue, on la crée
                    rtp::log::warning("Lazy spawning NetID: {}", snapshot.netId);
                    
                    rtp::net::EntitySpawnPayload fakeSpawn;
                    fakeSpawn.netId = snapshot.netId;
                    fakeSpawn.position = snapshot.position;
                    fakeSpawn.type = 0; // Type par défaut
                    handleEntitySpawn(fakeSpawn);
                }
            }

            // --- HELPER SPRITE ---
            void configureSprite(rtp::ecs::Entity entity, uint8_t type) {
                rtp::ecs::components::Sprite sprite;
                
                sprite.rectWidth = 0; sprite.rectHeight = 0; // Full image par défaut
                sprite.zIndex = 10;
                sprite.opacity = 255;
                sprite.red = 255; sprite.green = 255; sprite.blue = 255;

                switch (type) {
                    case 1: // Player
                        sprite.texturePath = "assets/sprites/players/image.png";
                        sprite.rectWidth = 33; sprite.rectHeight = 17;
                        break;
                    case 2: // Enemy Basic
                        sprite.texturePath = "assets/sprites/enemies/basic.png";
                        break;
                    case 3: // Projectile
                        sprite.texturePath = "assets/sprites/projectiles/beam.png";
                        break;
                    default: // Unknown
                        sprite.texturePath = "assets/sprites/debug.png";
                        break;
                }
                _r.addComponent<rtp::ecs::components::Sprite>(entity, sprite);
            }

            std::unordered_map<uint32_t, rtp::ecs::Entity> _serverToClientEntity;
            rtp::ecs::Registry& _r;
            rtp::client::ClientNetwork& _network;
    };
}  // namespace rtp::client

#endif /* !RTYPE_NETWORK_SYSTEM_HPP_ */