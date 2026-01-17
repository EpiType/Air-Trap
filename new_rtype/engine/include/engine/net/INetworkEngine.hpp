/**
 * File   : INetworkEngine.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#ifndef RTYPE_ENGINE_NET_INETWORKENGINE_HPP_
    #define RTYPE_ENGINE_NET_INETWORKENGINE_HPP_

    #include <cstdint>
    #include <string>
    #include "engine/net/INetwork.hpp"

/**
 * @namespace engine::net
 * @brief Networking components and structures for the engine
 */
namespace engine::net
{
    /**
     * @struct ClientConfig
     * @brief Configuration for creating a client network instance.
     */
    struct ClientConfig {
        std::string host{"127.0.0.1"};
        uint16_t tcpPort{0};
        uint16_t udpPort{0};
    };

    /**
     * @struct ServerConfig
     * @brief Configuration for creating a server network instance.
     */
    struct ServerConfig {
        std::string bindAddress{"0.0.0.0"};
        uint16_t tcpPort{0};
        uint16_t udpPort{0};
        uint32_t maxSessions{0};
    };

    /**
     * @class INetworkEngine
     * @brief Interface for network engine plugins (.so/.dll).
     * 
     * The network engine acts as a factory for client/server network instances.
     * Implementations live in plugins and are created through a C API entry point.
     */
    class INetworkEngine {
        public:
            virtual ~INetworkEngine() = default;

            /**
             * @brief Get the name of the network engine implementation.
             * @return Name of the network engine as a string
             */
            virtual std::string getName(void) const = 0;

            /**
             * @brief Create a client network instance.
             * @param config Client configuration
             * @return Pointer to a newly created INetwork instance
             */
            virtual INetwork *createClient(const ClientConfig &config) = 0;

            /**
             * @brief Create a server network instance.
             * @param config Server configuration
             * @return Pointer to a newly created INetwork instance
             */
            virtual INetwork *createServer(const ServerConfig &config) = 0;

            /**
             * @brief Destroy a network instance created by this engine.
             * @param network Pointer to the INetwork instance to destroy
             */
            virtual void destroy(INetwork *network) = 0;
    };
}

/**
 * @brief Entry points that network plugins must export.
 */
extern "C"
{
    engine::net::INetworkEngine *CreateNetworkEngine();
    void DestroyNetworkEngine(engine::net::INetworkEngine *engine);
}

#endif /* !RTYPE_ENGINE_NET_INETWORKENGINE_HPP_ */
