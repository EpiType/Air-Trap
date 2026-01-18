/**
 * File   : ClientApp.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#ifndef RTYPE_CLIENT_APP_HPP_
    #define RTYPE_CLIENT_APP_HPP_

    /* Engine */
    #include "engine/ecs/Registry.hpp"
    #include "engine/input/Input.hpp"
    #include "engine/net/INetwork.hpp"
    #include "engine/render/IRenderer.hpp"
    #include "engine/render/RenderFrame.hpp"
    #include "engine/scenes/IScene.hpp"

    /* R-Type Client */
    #include "rtype/utility/Settings.hpp"
    #include "rtype/utility/TranslationManager.hpp"
    #include "rtype/entity/EntityBuilder.hpp"
    #include "rtype/systems/AnimationSystem.hpp"
    #include "rtype/systems/NetworkSyncSystem.hpp"
    #include "rtype/systems/InputSystem.hpp"
    #include "rtype/systems/ParallaxSystem.hpp"
    #include "rtype/systems/RenderSystem.hpp"
    #include "rtype/systems/UISystem.hpp"
    #include "rtype/systems/UIRenderSystem.hpp"
    #include "rtype/scenes/LoginScene.hpp"
    #include "rtype/scenes/MenuScene.hpp"
    #include "rtype/scenes/LobbyScene.hpp"
    #include "rtype/utility/SceneId.hpp"

    #include <memory>
    #include <unordered_map>
    #include <vector>

#if defined(_WIN32)
    #define RTYPE_CLIENT_API __declspec(dllexport)
#else
    #define RTYPE_CLIENT_API __attribute__((visibility("default")))
#endif

namespace rtp::client
{
    /**
     * @class ClientApp
     * @brief Client application orchestrator.
     */
    class ClientApp
    {
        public:
            /**
             * @brief Constructor for ClientApp
             * @param renderer Renderer instance for graphics
             * @param network Network instance for communication
             */
            ClientApp(aer::render::IRenderer& renderer,
                      aer::net::INetwork& network);

            /** 
             * @brief Initialize the client application.
             */
            void init(void);
            
            /** 
             * @brief Shutdown the client application.
             */
            void shutdown(void);

            /** 
             * @brief Update the client application state.
             * @param dt Delta time since last update
             */
            void update(float dt);

            /**
             * @brief Build the render frame for this tick.
             * @param frame Output render frame
             */
            void render(aer::render::RenderFrame& frame);

            /** 
             * @brief Render the current frame.
             */
            void setScene(SceneId sceneId);

            /**
             * @brief Handle input events.
             * @param event Input event to handle
             */
            void handleEvents(const aer::input::Event& event);

        private:
            /** 
             * @brief Create and initialize all scenes.
             */
            void createScenes(void);

            /**
             * @brief Register ECS components used by client systems.
             */
            void registerComponents(void);

        private:
            aer::render::IRenderer& _renderer;                   /**< Renderer instance */
            aer::net::INetwork& _network;                        /**< Network instance */

            aer::ecs::Registry _worldRegistry;                   /**< World ECS registry */
            aer::ecs::Registry _uiRegistry;                      /**< UI ECS registry */
            aer::render::RenderFrame _worldFrame;                /**< Frame for world rendering */
            aer::render::RenderFrame _uiFrame;                   /**< Frame for UI rendering */

            Settings _settings;                                     /**< Client settings manager */
            TranslationManager _translation;                        /**< Translation manager */

            NetworkSyncSystem _networkSystem;                       /**< Network synchronization system */
            systems::InputSystem _inputSystem;                      /**< Input handling system */
            systems::RenderSystem _renderSystem;                    /**< Rendering system for game entities */
            systems::UIRenderSystem _uiRenderSystem;                /**< Rendering system for UI components */
            systems::UISystem _uiSystem;                            /**< UI management system */
            systems::ParallaxSystem _parallaxSystem;                /**< Parallax background system */
            systems::AnimationSystem _animationSystem;              /**< Animation system for handling entity animations */

            std::unordered_map<SceneId,
                std::unique_ptr<aer::scenes::IScene>> _scenes;   /**< All available scenes */
            SceneId _activeScene{SceneId::Login};                   /**< Currently active scene */
            bool _sceneActive{false};                               /**< True once onEnter ran */
    };
}

extern "C"
{
    RTYPE_CLIENT_API rtp::client::ClientApp *CreateClientApp(
        aer::render::IRenderer *renderer,
        aer::net::INetwork *network);
    RTYPE_CLIENT_API void DestroyClientApp(rtp::client::ClientApp *app);
}

#endif /* !RTYPE_CLIENT_APP_HPP_ */
