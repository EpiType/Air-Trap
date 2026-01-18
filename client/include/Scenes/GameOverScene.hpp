/**
 * File   : GameOverScene.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_CLIENT_SCENES_GAMEOVERSCENE_HPP_
    #define RTYPE_CLIENT_SCENES_GAMEOVERSCENE_HPP_

    #include "Interfaces/IScene.hpp"
    #include "RType/ECS/Registry.hpp"
    #include "UI/UiFactory.hpp"
    #include "Utils/GameState.hpp"
    #include "Systems/NetworkSyncSystem.hpp"
    #include "Translation/TranslationManager.hpp"
    #include "Core/Settings.hpp"
    #include <SFML/Graphics.hpp>
    #include <functional>

namespace rtp::client {
    namespace scenes {
        class GameOverScene : public interfaces::IScene
        {
            public:
                GameOverScene(ecs::Registry& UiRegistry,
                              Settings& settings,
                              TranslationManager& translationManager,
                              NetworkSyncSystem& network,
                              graphics::UiFactory& uiFactory,
                              std::function<void(GameState)> changeState);

                using ChangeStateFn = std::function<void(GameState)>;

                void onEnter(void) override;
                void onExit(void) override;
                void handleEvent(const sf::Event& event) override;
                void update(float dt) override;

            private:
                ecs::Registry& _uiRegistry;
                Settings& _settings;
                TranslationManager& _translationManager;
                NetworkSyncSystem& _network;
                graphics::UiFactory& _uiFactory;
                ChangeStateFn _changeState;
        };
    } // namespace scenes
} // namespace rtp::client

#endif // RTYPE_CLIENT_SCENES_GAMEOVERSCENE_HPP_
