/**
 * File   : AServerModule.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#ifndef ENGINE_GAME_ASERVERMODULE_HPP_
    #define ENGINE_GAME_ASERVERMODULE_HPP_

    #include "engine/game/IGameModule.hpp"

/**
 * @namespace engine::game
 * @brief Game module interface for the engine
 * @note This abstract class provides a base implementation for game modules
 *       that can be used in both client and server contexts.
 *      It implements the IGameModule interface and can be extended by
 *      specific game module implementations.
 */
namespace engine::game
{
    /**
     * @class AServerModule
     * @brief Abstract base class for server game modules.
     * 
     * This class provides a base implementation for game modules
     * that operate in a server context. It inherits from IGameModule
     * and can be extended by specific server game module implementations.
     */
    class AServerModule : public IGameModule {
        public:
            virtual ~AServerModule() = default;

            //////////////////////////////////////////////////////////////////////////////
            // Server API
            //////////////////////////////////////////////////////////////////////////////

            virtual bool initServer() override = 0;

            virtual void updateServer(float dt) override = 0;

            virtual void shutdownServer() override = 0;

            //////////////////////////////////////////////////////////////////////////////
            // Client API - Not allowed to use as server module
            //////////////////////////////////////////////////////////////////////////////

            virtual bool initClient() override { return false; };

            virtual void updateClient(float dt) override { (void)dt; };

            virtual void renderClient(render::RenderFrame &frame) override { (void)frame; };

            virtual void onClientEvents(const std::vector<input::Event> &events) override { (void)events; };

            virtual void shutdownClient() override {};
    };
} // namespace engine::game

#endif /* !ENGINE_GAME_ASERVERMODULE_HPP_ */