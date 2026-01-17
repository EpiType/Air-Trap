/**
 * File   : LobbyScene.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#ifndef RTYPE_CLIENT_LOBBY_SCENE_HPP_
    #define RTYPE_CLIENT_LOBBY_SCENE_HPP_

    /* Engine */
    #include "engine/scenes/IScene.hpp"
    #include "engine/ecs/Registry.hpp"
    #include "engine/ui/UiFactory.hpp"

    /* R-Type Client */
    #include "rtype/utility/Settings.hpp"
    #include "rtype/utility/TranslationManager.hpp"
    #include "rtype/systems/NetworkSyncSystem.hpp"

    #include <functional>
    #include <string>

namespace rtp::client::scenes
{
    /**
     * @class LobbyScene
     * @brief Scene for the lobby and room list.
     */
    class LobbyScene : public engine::scenes::IScene
    {
        public:
            LobbyScene(engine::ecs::Registry& uiRegistry,
                       Settings& settings,
                       TranslationManager& translation,
                       NetworkSyncSystem& network,
                       std::function<void()> onBack = {});

            void onEnter(void) override;
            void onExit(void) override;
            void handleEvent(const engine::input::Event &event) override;
            void update(float dt) override;

        private:
            engine::ecs::Registry& _uiRegistry;
            Settings& _settings;
            TranslationManager& _translation;
            NetworkSyncSystem& _network;
            std::function<void()> _onBack;

            std::string _roomName{"Room"};
    };
}

#endif /* !RTYPE_CLIENT_LOBBY_SCENE_HPP_ */
