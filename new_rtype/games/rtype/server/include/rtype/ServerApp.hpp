/**
 * File   : ServerApp.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#ifndef RTYPE_SERVER_APP_HPP_
    #define RTYPE_SERVER_APP_HPP_

    #include "engine/net/INetworkEngine.hpp"
    #include "engine/plugin/LibraryManager.hpp"
    #include "rtype/game/GameManager.hpp"
    #include "server/include/rtype/net/NetworkBootstrap.hpp"

    #include <chrono>
    #include <memory>
    #include <string>

namespace rtp::server
{
    class ServerApp {
        public:
            /** 
             * @struct Config
             * @brief Configuration parameters for the server application.
             */
            struct Config {
                std::string networkPluginPath{};
                std::string bindAddress{"0.0.0.0"};
                uint16_t tcpPort{4242};
                uint16_t udpPort{4243};
                uint32_t maxSessions{64};
                double tickRate{60.0};
            };

        public:
            /** 
             * @brief Constructor for ServerApp
             * @param config Configuration parameters
             */
            explicit ServerApp(Config config);

            /** 
             * @brief Initialize the server application.
             * @return True if initialization was successful, false otherwise
             */
            bool init(void);

            /** 
             * @brief Run the server application.
             */
            void run(void);

            /** 
             * @brief Stop the server application.
             */
            void stop(void);

            /** 
             * @brief Shutdown the server application.
             */
            void shutdown(void);

        private:
            /** 
             * @brief Load the network engine plugin.
             * @return True if the network engine was loaded successfully, false otherwise
             */
            bool loadNetworkEngine(void);

        private:
            Config _config{};
            engine::plugin::LibraryManager _libraries;
            std::shared_ptr<engine::plugin::DynamicLibrary> _networkLib{};
            engine::net::INetworkEngine *_networkEngine{nullptr};
            std::unique_ptr<rtp::server::net::NetworkBootstrap> _network;
            std::unique_ptr<GameManager> _gameManager;
            bool _running{false};
        };
}

#endif /* !RTYPE_SERVER_APP_HPP_ */
