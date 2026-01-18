/**
 * File   : AClientModule.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#ifndef ENGINE_GAME_ACLIENTMODULE_HPP_
    #define ENGINE_GAME_ACLIENTMODULE_HPP_

    #include "engine/game/IGameModule.hpp"

/**
 * @namespace aer::game
 * @brief Game module interface for the engine
 * @note This abstract class provides a base implementation for game modules
 *       that can be used in both client and server contexts.
 *      It implements the IGameModule interface and can be extended by
 *      specific game module implementations.
 */
namespace aer::game
{
    /**
     * @class AClientModule
     * @brief Abstract base class for client game modules.
     * 
     * This class provides a base implementation for game modules
     * that operate in a client context. It inherits from IGameModule
     * and can be extended by specific client game module implementations.
     */
    class AClientModule : public IGameModule {
        public:
            virtual ~AClientModule() = default;

            //////////////////////////////////////////////////////////////////////////////
            // Client API
            //////////////////////////////////////////////////////////////////////////////

            virtual bool initClient() override = 0;

            virtual void updateClient(float dt) override = 0;

            virtual void renderClient(render::RenderFrame &frame) override = 0;

            virtual void onClientEvents(const std::vector<input::Event> &events) override = 0;

            virtual void shutdownClient() override = 0;

            //////////////////////////////////////////////////////////////////////////////
            // Server API - Not allowed to use as client module
            //////////////////////////////////////////////////////////////////////////////

            virtual bool initServer() override { return false; };

            virtual void updateServer(float dt) override { (void)dt; };

            virtual void shutdownServer() override {};
    };
} // namespace aer::game

#endif /* !ENGINE_GAME_AClientModule_HPP_ */
