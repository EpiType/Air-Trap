/**
 * File   : PauseScene.hpp
 * License: MIT
 * Author : Elias Josue HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#ifndef RTYPE_CLIENT_PAUSE_SCENE_HPP_
    #define RTYPE_CLIENT_PAUSE_SCENE_HPP_

    /* Engine */
    #include "engine/scenes/IScene.hpp"
    #include "engine/ecs/Registry.hpp"
    #include "engine/ui/UiFactory.hpp"

    /* R-Type Client */
    #include "rtype/utility/Settings.hpp"
    #include "rtype/utility/TranslationManager.hpp"
    #include "rtype/utility/SceneId.hpp"
    #include "rtype/systems/NetworkSyncSystem.hpp"

    #include <functional>

namespace rtp::client::scenes
{
    class PauseScene : public aer::scenes::IScene
    {
        public:
            PauseScene(aer::ecs::Registry& uiRegistry,
                       Settings& settings,
                       TranslationManager& translation,
                       NetworkSyncSystem& network,
                       std::function<void(SceneId)> changeScene);

            void onEnter(void) override;
            void onExit(void) override;
            void handleEvent(const aer::input::Event &event) override;
            void update(float dt) override;

        private:
            aer::ecs::Registry& _uiRegistry;
            Settings& _settings;
            TranslationManager& _translation;
            NetworkSyncSystem& _networkSystem;
            std::function<void(SceneId)> _changeScene;
    };
}

#endif /* !RTYPE_CLIENT_PAUSE_SCENE_HPP_ */
