/**
 * File   : IGameModule.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#ifndef ENGINE_GAME_IGAMEMODULE_HPP_
    #define ENGINE_GAME_IGAMEMODULE_HPP_

    #include "engine/input/Event.hpp"
    #include "engine/render/RenderFrame.hpp"

    #include <vector>

/**
 * @namespace engine::game
 * @brief Game module interface for the engine
 * @note This interface defines the contract for game it can be used
 *       in both client and server contexts.
 */
namespace engine::game
{
    /**
     * @class IGameModule
     * @brief Interface for game modules.
     * 
     * This interface defines the contract for game modules that can be
     * integrated into the engine. Implementations must provide methods
     * for initializing, updating, rendering (client-side), handling
     * input events (client-side), and shutting down the module.
     */
    class IGameModule {
        public:
            virtual ~IGameModule() = default;

            //////////////////////////////////////////////////////////////////////////////
            // Client API
            //////////////////////////////////////////////////////////////////////////////
            /** 
             * @brief Initialize the game module for client context.
             * @return true if initialization was successful, false otherwise
             */
            virtual bool initClient() = 0;

            /** 
             * @brief Update the game module logic for client context.
             * @param dt Time elapsed since last update in seconds
             */
            virtual void updateClient(float dt) = 0;

            /** 
             * @brief Render the current frame for client context.
             * @param frame Reference to the RenderFrame to populate
             */
            virtual void renderClient(render::RenderFrame &frame) = 0;

            /** 
             * @brief Handle input events for client context.
             * @param events Vector of input events to process
             */
            virtual void onClientEvents(const std::vector<input::Event> &events) = 0;

            /** 
             * @brief Shutdown the game module for client context.
             */
            virtual void shutdownClient() = 0;

            //////////////////////////////////////////////////////////////////////////////
            // Server API
            //////////////////////////////////////////////////////////////////////////////

            /** 
             * @brief Initialize the game module for server context.
             * @return true if initialization was successful, false otherwise
             */
            virtual bool initServer() = 0;

            /** 
             * @brief Update the game module logic for server context.
             * @param dt Time elapsed since last update in seconds
             */
            virtual void updateServer(float dt) = 0;
            
            /** 
             * @brief Shutdown the game module for server context.
             */
            virtual void shutdownServer() = 0;
    };
}

#endif /* !ENGINE_GAME_IGAMEMODULE_HPP_ */
