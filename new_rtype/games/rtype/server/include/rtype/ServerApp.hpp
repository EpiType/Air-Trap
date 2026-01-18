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

#if defined(_WIN32)
    #define RTYPE_SERVER_API __declspec(dllexport)
#else
    #define RTYPE_SERVER_API __attribute__((visibility("default")))
#endif

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
            aer::plugin::LibraryManager _libraries;
            std::shared_ptr<aer::plugin::DynamicLibrary> _networkLib{};
            aer::net::INetworkEngine *_networkEngine{nullptr};
            std::unique_ptr<rtp::server::net::NetworkBootstrap> _network;
            std::unique_ptr<GameManager> _gameManager;
            bool _running{false};
        };
}

extern "C"
{
    RTYPE_SERVER_API rtp::server::ServerApp *CreateServerApp(
        rtp::server::ServerApp::Config config);
    RTYPE_SERVER_API void DestroyServerApp(rtp::server::ServerApp *app);
    RTYPE_SERVER_API rtp::server::ServerApp *CreateServerAppWithConfig(
        const char *networkPluginPath,
        const char *bindAddress,
        std::uint16_t tcpPort,
        std::uint16_t udpPort,
        std::uint32_t maxSessions,
        double tickRate);
    RTYPE_SERVER_API bool ServerAppInit(rtp::server::ServerApp *app);
    RTYPE_SERVER_API void ServerAppRun(rtp::server::ServerApp *app);
    RTYPE_SERVER_API void ServerAppStop(rtp::server::ServerApp *app);
    RTYPE_SERVER_API void ServerAppShutdown(rtp::server::ServerApp *app);
}

#endif /* !RTYPE_SERVER_APP_HPP_ */
