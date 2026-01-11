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

        PlayingScene::PlayingScene(ecs::Registry& UiRegistry,
                                   Settings& settings,
                                   TranslationManager& translationManager,
                                   NetworkSyncSystem& network,
                                   graphics::UiFactory& uiFactory,
                                   std::function<void(GameState)> changeState)
            : _uiRegistry(UiRegistry),
              _settings(settings),
              _translationManager(translationManager),
              _network(network),
              _uiFactory(uiFactory),
              _changeState(changeState)
        {
        }

        void PlayingScene::onEnter(void)
        {
            rtp::log::info("Entering PlayingScene");

            // _uiFactory.createText(
            //     _uiRegistry,
            //     {10.0f, 10.0f},
            //     _translationManager.get("game.playing"),
            //     "assets/fonts/main.ttf",
            //     24,
            //     5,
            //     {200, 255, 200}
            // );
        }

        void PlayingScene::onExit(void)
        {
            _uiRegistry.clear();
        }


        void PlayingScene::handleEvent(const sf::Event& event)
        {
            (void)event;
        }

        void PlayingScene::update(float dt)
        {
            (void)dt;
        }
    } // namespace Scenes
} // namespace rtp::client