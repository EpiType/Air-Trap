/**
 * File   : RoomWaitingScene.hpp
 * License: MIT
 * Author : Elias Josue HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#ifndef RTYPE_CLIENT_ROOM_WAITING_SCENE_HPP_
    #define RTYPE_CLIENT_ROOM_WAITING_SCENE_HPP_

    /* Engine */
    #include "engine/scenes/IScene.hpp"
    #include "engine/ecs/Registry.hpp"
    #include "engine/ecs/Entity.hpp"
    #include "engine/ui/UiFactory.hpp"

    /* R-Type Client */
    #include "rtype/utility/Settings.hpp"
    #include "rtype/utility/TranslationManager.hpp"
    #include "rtype/utility/SceneId.hpp"
    #include "rtype/systems/NetworkSyncSystem.hpp"

    #include <functional>

namespace rtp::client::scenes
{
    class RoomWaitingScene : public aer::scenes::IScene
    {
        public:
            RoomWaitingScene(aer::ecs::Registry& uiRegistry,
                             Settings& settings,
                             TranslationManager& translation,
                             NetworkSyncSystem& network,
                             std::function<void(SceneId)> changeScene);

            void onEnter(void) override;
            void onExit(void) override;
            void handleEvent(const aer::input::Event &event) override;
            void update(float dt) override;

        private:
            void openChat(void);
            void closeChat(void);
            void updateChatHistoryText(void);
            void sendChatMessage(void);
            void updateReadyButtonText(void);

            aer::ecs::Registry& _uiRegistry;
            Settings& _settings;
            TranslationManager& _translation;
            NetworkSyncSystem& _networkSystem;
            std::function<void(SceneId)> _changeScene;

            bool _chatOpen{false};
            bool _uiReady{false};

            aer::ecs::Entity _chatCompactPanel{};
            aer::ecs::Entity _chatCompactText{};
            aer::ecs::Entity _chatToggleButton{};
            aer::ecs::Entity _chatPanel{};
            aer::ecs::Entity _chatHistoryText{};
            aer::ecs::Entity _chatInput{};
            aer::ecs::Entity _readyButton{};
    };
}

#endif /* !RTYPE_CLIENT_ROOM_WAITING_SCENE_HPP_ */
