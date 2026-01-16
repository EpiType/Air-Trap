/**
 * File   : ANetwork.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#ifndef ENGINE_NET_ANETWORK_HPP_
    #define ENGINE_NET_ANETWORK_HPP_

    #include "engine/net/INetwork.hpp"

    #include <deque>
    #include <mutex>
    #include <optional>

/**
 * @namespace engine::net
 * @brief Networking components and structures for the engine
 */
namespace engine::net
{
    /**
     * @class ANetwork
     * @brief Abstract base class for server network implementations.
     * Provides an event queue and a default poll implementation.
     */
    class ANetwork : public INetwork {
        public:
            virtual ~ANetwork() = default;

            /**
             * @brief Poll for incoming network events.
             * @return An optional NetworkEvent if an event is available,
             *         std::nullopt otherwise
             */
            std::optional<NetworkEvent> pollEvent(void) override;

        protected:
            /**
             * @brief Push a new network event into the event queue.
             * @param event The NetworkEvent to push
             */
            void pushEvent(NetworkEvent event);

        private:
            std::mutex _eventMutex;             /**< Mutex for thread-safe event queue access */
            std::deque<NetworkEvent> _events;   /**< Queue of incoming network events */
    };
}

#endif /* !ENGINE_NET_ANETWORK_HPP_ */
