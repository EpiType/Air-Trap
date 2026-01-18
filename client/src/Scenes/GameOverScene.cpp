/**
 * File   : GameOverScene.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Scenes/GameOverScene.hpp"

namespace rtp::client {
    namespace scenes {

        GameOverScene::GameOverScene(ecs::Registry& UiRegistry,
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

        void GameOverScene::onEnter(void)
        {
            log::info("Entering GameOverScene");

            const auto summary = _network.getGameOverSummary();
            const std::string bestName = summary.bestPlayer.empty() ? "Player" : summary.bestPlayer;

            _uiFactory.createText(
                _uiRegistry,
                {468.0f, 160.0f},
                "GAME OVER",
                "assets/fonts/title.ttf",
                64,
                10,
                {255, 120, 120}
            );

            _uiFactory.createText(
                _uiRegistry,
                {360.0f, 290.0f},
                bestName + " has best score " + std::to_string(summary.bestScore),
                "assets/fonts/main.ttf",
                22,
                10,
                {230, 230, 230}
            );

            _uiFactory.createText(
                _uiRegistry,
                {360.0f, 330.0f},
                "Your score: " + std::to_string(summary.playerScore),
                "assets/fonts/main.ttf",
                20,
                10,
                {180, 180, 180}
            );

            _uiFactory.createButton(
                _uiRegistry,
                {490.0f, 420.0f},
                {300.0f, 60.0f},
                "BACK TO MENU",
                [this]() {
                    _network.tryLeaveRoom();
                    _changeState(GameState::Menu);
                }
            );
        }

        void GameOverScene::onExit(void)
        {
            log::info("Exiting GameOverScene");
        }

        void GameOverScene::handleEvent(const sf::Event& event)
        {
            if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {
                if (kp->code == sf::Keyboard::Key::Enter ||
                    kp->code == sf::Keyboard::Key::Escape) {
                    _network.tryLeaveRoom();
                    _changeState(GameState::Menu);
                }
            }
        }

        void GameOverScene::update(float dt)
        {
            (void)dt;
        }

    } // namespace scenes
} // namespace rtp::client
