/**
 * File   : LoginScene.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#ifndef RTYPE_CLIENT_LOGIN_SCENE_HPP_
    #define RTYPE_CLIENT_LOGIN_SCENE_HPP_

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
     * @class LoginScene
     * @brief Scene responsible for login/register UI.
     */
    class LoginScene : public aer::scenes::IScene
    {
        public:
            /**
             * @brief Constructor for LoginScene
             * @param uiRegistry UI registry used for this scene
             * @param settings Client settings
             * @param translation Translation manager
             * @param network Client network system
             * @param onLoggedIn Callback when login succeeds
             */
            LoginScene(aer::ecs::Registry& uiRegistry,
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

            std::string _username;
            std::string _password;
            bool _loginNotified{false};
    };
}

#endif /* !RTYPE_CLIENT_LOGIN_SCENE_HPP_ */
