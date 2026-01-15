/**
 * File   : GamepadSettingsScene.hpp
 * License: MIT
 * Author : GitHub Copilot
 * Date   : 13/01/2026
 */

#ifndef RTYPE_CLIENT_SCENES_GAMEPADSETTINGSSCENE_HPP_
    #define RTYPE_CLIENT_SCENES_GAMEPADSETTINGSSCENE_HPP_

    #include "Interfaces/IScene.hpp"
    #include "RType/ECS/Registry.hpp"
    #include "UI/UiFactory.hpp"
    #include "Utils/GameState.hpp"
    #include "Systems/NetworkSyncSystem.hpp"
    #include "Translation/TranslationManager.hpp"
    #include "Core/Settings.hpp"
    #include <SFML/Graphics.hpp>
    #include <memory>
    #include <functional>
    #include <unordered_map>

namespace rtp::client {
    namespace scenes {
        
        enum class GamepadAction {
            Shoot,
            Reload,
            Pause
        };
        
        /**
         * @class GamepadSettingsScene
         * @brief Scene for configuring gamepad settings.
         */
        class GamepadSettingsScene : public interfaces::IScene
        {
            public:
                GamepadSettingsScene(ecs::Registry& UiRegistry,
                                    Settings& settings,
                                    TranslationManager& translationManager,
                                    NetworkSyncSystem& network,
                                    graphics::UiFactory& uiFactory,
                                    std::function<void(GameState)> changeState);

                using ChangeStateFn = std::function<void(GameState)>;

                void onEnter() override;
                void onExit() override;
                void handleEvent(const sf::Event& event) override;
                void update(float dt) override;

            private:
                void refreshButtonLabel(GamepadAction action);
                std::string getButtonName(unsigned int button) const;
                
                ecs::Registry& _uiRegistry;
                Settings& _settings;
                TranslationManager& _translationManager;
                NetworkSyncSystem& _network;
                graphics::UiFactory& _uiFactory;
                ChangeStateFn _changeState;
                
                bool _isWaitingForButton{false};
                GamepadAction _actionToRebind;
                std::unordered_map<GamepadAction, ecs::Entity> _actionToButton;
        };
    } // namespace scenes
} // namespace rtp::client

#endif // RTYPE_CLIENT_SCENES_GAMEPADSETTINGSSCENE_HPP_
