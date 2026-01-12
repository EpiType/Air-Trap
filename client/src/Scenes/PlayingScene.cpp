/**
 * File   : PlayingScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Scenes/PlayingScene.hpp"

namespace rtp::client {
    namespace Scenes {

        //////////////////////////////////////////////////////////////////////////
        // Public API
        //////////////////////////////////////////////////////////////////////////

        PlayingScene::PlayingScene(ecs::Registry& worldRegistry,
                                    ecs::Registry& uiRegistry,
                                    Settings& settings,
                                    TranslationManager& translationManager,
                                    NetworkSyncSystem& network,
                                    graphics::UiFactory& uiFactory,
                                    EntityBuilder& worldBuilder,
                                    std::function<void(GameState)> changeState)
            : _uiRegistry(uiRegistry),
              _worldRegistry(worldRegistry),
              _settings(settings),
              _translationManager(translationManager),
              _network(network),
              _uiFactory(uiFactory),
              _worldBuilder(worldBuilder),
              _changeState(changeState)
        {
        }

        void PlayingScene::onEnter()
        {
            rtp::log::info("Entering PlayingScene");

            spawnParallax();

            auto makeHudText = [&](const rtp::Vec2f& pos, unsigned size) -> rtp::ecs::Entity {
                auto eRes = _uiRegistry.spawnEntity();
                if (!eRes) return {};

                auto e = eRes.value();
                rtp::ecs::components::ui::Text t;
                t.content = "";
                t.position = pos;
                t.fontPath = "assets/fonts/main.ttf";
                t.fontSize = size;
                t.red = 255; t.green = 255; t.blue = 255;
                t.alpha = 255;
                t.zIndex = 999;
                _uiRegistry.addComponent<rtp::ecs::components::ui::Text>(e, t);
                return e;
            };

            _hudPing     = makeHudText({980.0f, 15.0f}, 18);
            _hudFps      = makeHudText({980.0f, 40.0f}, 18);
            _hudScore    = makeHudText({980.0f, 65.0f}, 20);
            _hudEntities = makeHudText({980.0f, 90.0f}, 18);

            _hudInit = true;
        }

        void PlayingScene::onExit(void)
        {
            _hudInit = false;
            _uiRegistry.clear();
        }


        void PlayingScene::handleEvent(const sf::Event& e)
        {
            if (const auto* kp = e.getIf<sf::Event::KeyPressed>()) {
                if (kp->code == sf::Keyboard::Key::Escape) {
                    _changeState(GameState::Paused);
                }
            }
        }

        void PlayingScene::update(float dt)
        {
            (void)dt;
            if (!_hudInit) return;

            auto textsOpt = _uiRegistry.getComponents<rtp::ecs::components::ui::Text>();
            if (!textsOpt) return;
            auto& texts = textsOpt.value().get();

            if (texts.has(_hudPing))     texts[_hudPing].content = "Ping: " + std::to_string(_uiPing) + " ms";
            if (texts.has(_hudFps))      texts[_hudFps].content  = "FPS: "  + std::to_string(_uiFps);
            if (texts.has(_hudScore))    texts[_hudScore].content = "Score: " + std::to_string(_uiScore);

            auto spritesOpt = _worldRegistry.getComponents<rtp::ecs::components::Sprite>();
            if (spritesOpt && texts.has(_hudEntities)) {
                texts[_hudEntities].content = "Entities: " + std::to_string(spritesOpt.value().get().size());
            }
        }

        ////////////////////////////////////////////////////////////////////////
        // Private API
        ////////////////////////////////////////////////////////////////////////

        void PlayingScene::spawnParallax()
        {
            constexpr float baseTextureWidth = 1280.0f;

            auto spawnLayer = [&](EntityTemplate t) {
                const float scaledW = baseTextureWidth * t.scale.x;

                t.position.x = 0.0f;
                auto a = _worldBuilder.spawn(t);
                if (!a) {
                    rtp::log::error("Failed to spawn parallax A");
                    return;
                }

                t.position.x = scaledW;
                auto b = _worldBuilder.spawn(t);
                if (!b) {
                    rtp::log::error("Failed to spawn parallax B");
                    return;
                }
            };

            spawnLayer(EntityTemplate::createParrallaxLayer1());
            spawnLayer(EntityTemplate::createParrallaxLayer2());
        }

    } // namespace Scenes
} // namespace rtp::client