/**
 * File   : MenuScene.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#ifndef RTYPE_CLIENT_MENU_SCENE_HPP_
    #define RTYPE_CLIENT_MENU_SCENE_HPP_

    /* Engine */
    #include "engine/scenes/IScene.hpp"
    #include "engine/ecs/Registry.hpp"
    #include "engine/ui/UiFactory.hpp"

    /* R-Type Client */
    #include "rtype/utility/Settings.hpp"
    #include "rtype/utility/TranslationManager.hpp"
    #include "rtype/systems/NetworkSyncSystem.hpp"
    #include "rtype/utility/SceneId.hpp"

    #include <functional>
    #include <string>

namespace rtp::client::scenes
{
    /**
     * @class MenuScene
     * @brief Scene for the main menu.
     */
    class MenuScene : public aer::scenes::IScene
    {
        public:
            MenuScene(aer::ecs::Registry& uiRegistry,
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

#endif /* !RTYPE_CLIENT_MENU_SCENE_HPP_ */
