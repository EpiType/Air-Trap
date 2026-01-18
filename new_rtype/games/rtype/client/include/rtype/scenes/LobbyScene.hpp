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
    #include "rtype/utility/SceneId.hpp"

    #include <cstddef>
    #include <functional>
    #include <list>
    #include <string>
    #include <string>

namespace rtp::client::scenes
{
    /**
     * @class LobbyScene
     * @brief Scene for the lobby and room list.
     */
    class LobbyScene : public aer::scenes::IScene
    {
        public:
            LobbyScene(aer::ecs::Registry& uiRegistry,
                       Settings& settings,
                       TranslationManager& translation,
                       NetworkSyncSystem& network,
                       std::function<void(SceneId)> changeScene);

            void onEnter(void) override;
            void onExit(void) override;
            void handleEvent(const aer::input::Event &event) override;
            void update(float dt) override;

        private:
            void buildUi(void);

            aer::ecs::Registry& _uiRegistry;
            Settings& _settings;
            TranslationManager& _translation;
            NetworkSyncSystem& _networkSystem;
            std::function<void(SceneId)> _changeScene;

            std::string _roomName{"Room"};
            std::size_t _roomsHash{0};
            bool _uiBuilt{false};
            uint32_t _uiSelectedRoomId{0};
    };
}

#endif /* !RTYPE_CLIENT_LOBBY_SCENE_HPP_ */
