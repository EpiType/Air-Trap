/**
 * File   : AsioEngine.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#ifndef ENGINE_NET_ASIO_ASIOENGINE_HPP_
    #define ENGINE_NET_ASIO_ASIOENGINE_HPP_

    #include "engine/net/INetworkEngine.hpp"
    #include <string>

/**
 * @namespace aer::net::asio
 * @brief ASIO-based networking engine implementation
 */
namespace aer::net::asio
{
    /**
     * @class AsioEngine
     * @brief ASIO implementation of the INetworkEngine interface.
     */
    class AsioEngine final : public INetworkEngine {
        public:
            /**
             * @brief Destructor for AsioEngine
             */
            ~AsioEngine() override = default;

            /**
             * @brief Get the name of the network engine implementation.
             * @return Name of the network engine as a string
             */
            std::string getName(void) const override;

            /**
             * @brief Create a client network instance.
             * @param config Client configuration
             * @return Pointer to a newly created INetwork instance
             */
            INetwork *createClient(const ClientConfig &config) override;

            /**
             * @brief Create a server network instance.
             * @param config Server configuration
             * @return Pointer to a newly created INetwork instance
             */
            INetwork *createServer(const ServerConfig &config) override;

            /**
             * @brief Destroy a network instance created by this engine.
             * @param network Pointer to the INetwork instance to destroy
             */
            void destroy(INetwork *network) override;
    };
} // namespace aer::net::asio

#endif /* !ENGINE_NET_ASIO_ASIOENGINE_HPP_ */
