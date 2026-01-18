/**
 * File   : KeyBindingScene.hpp
 * License: MIT
 * Author : Elias Josue HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#ifndef RTYPE_CLIENT_KEY_BINDING_SCENE_HPP_
    #define RTYPE_CLIENT_KEY_BINDING_SCENE_HPP_

    /* Engine */
    #include "engine/scenes/IScene.hpp"
    #include "engine/ecs/Registry.hpp"
    #include "engine/ui/UiFactory.hpp"

    /* R-Type Client */
    #include "rtype/utility/Settings.hpp"
    #include "rtype/utility/TranslationManager.hpp"
    #include "rtype/utility/KeyAction.hpp"
    #include "rtype/utility/SceneId.hpp"
    #include "rtype/systems/NetworkSyncSystem.hpp"

    #include <functional>
    #include <unordered_map>

namespace rtp::client::scenes
{
    class KeyBindingScene : public aer::scenes::IScene
    {
        public:
            KeyBindingScene(aer::ecs::Registry& uiRegistry,
                            Settings& settings,
                            TranslationManager& translation,
                            NetworkSyncSystem& network,
                            std::function<void(SceneId)> changeScene);

            void onEnter(void) override;
            void onExit(void) override;
            void handleEvent(const aer::input::Event &event) override;
            void update(float dt) override;

        private:
            struct KeyActionHash {
                std::size_t operator()(KeyAction action) const noexcept
                {
                    return static_cast<std::size_t>(action);
                }
            };

            void refreshButtonLabel(KeyAction action);

            aer::ecs::Registry& _uiRegistry;
            Settings& _settings;
            TranslationManager& _translation;
            NetworkSyncSystem& _networkSystem;
            std::function<void(SceneId)> _changeScene;

            bool _isWaitingForKey{false};
            KeyAction _actionToRebind{KeyAction::MoveUp};
            std::unordered_map<KeyAction, aer::ecs::Entity, KeyActionHash> _actionToButton;
    };
}

#endif /* !RTYPE_CLIENT_KEY_BINDING_SCENE_HPP_ */
