/**
 * File   : Core.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#ifndef RTYPE_CORE_HPP_
    #define RTYPE_CORE_HPP_

    #include "engine/log/LogLevel.hpp"
    #include "engine/log/Logger.hpp"
    #include "engine/core/Time.hpp"
    #include "engine/log/Error.hpp"
    #include "engine/plugin/LibraryManager.hpp"
    #include "engine/net/INetworkEngine.hpp"
    #include "engine/render/IRenderer.hpp"
    #include "engine/render/RenderFrame.hpp"

    #include <functional>
    #include <memory>
    #include <string>
    #include <set>

namespace aer::core
{
    class Core {
        public:
            /**
             * @struct Config
             * @brief Configuration parameters for initializing the Core
             */
            struct Config {
                std::vector<std::string> availableRenderers{};
                std::vector<std::string> availableNetworkEngines{};
                std::string defaultRendererPluginPath{};
                std::string defaultNetworkPluginPath{};
                std::string rendererPluginPath;
                std::string networkPluginPath;
                int width;
                int height;
                std::string title;
                std::string assetsPath;
                std::string networkIp;
                std::uint16_t networkPort;
            };

            /**
             * @brief Get the singleton instance of the Core
             * @return Reference to the Core instance
             */
            static Core& instance();

            /**
             * @brief Initialize the core with the specified configuration.
             * @param config Configuration parameters for initialization
             * @return true if initialization was successful, false otherwise
             */
            bool init(const Config& config);

            /**
             * @brief Start the core's main loop.
             */
            void run();

            /**
             * @brief Stop the core's main loop.
             */
            void stop();

            /**
             * @brief Shutdown the core and release resources.
             */
            void shutdown();

            /**
             * @brief Get the library manager instance
             * @return Reference to the LibraryManager instance
             */
            plugin::LibraryManager& libraries();

            /**
             * @brief Load a dynamic library from the specified path
             * @param path Path to the dynamic library
             * @return Shared pointer to the loaded DynamicLibrary
             */
            [[nodiscard]]
            auto loadLibrary(const std::string& path)
                -> std::expected<std::shared_ptr<plugin::DynamicLibrary>, core::Error>;

            /**
             * @brief Load a renderer plugin
             * @param path Path to the renderer plugin
             */
            [[nodiscard]]
            bool loadRendererPlugin(const std::string& path);

            /**
             * @brief Load a network engine plugin
             * @param path Path to the network engine plugin
             */
            [[nodiscard]]
            bool loadNetworkPlugin(const std::string& path);

            /**
             * @brief Get the current network engine
             * @return Pointer to the active INetworkEngine instance
             */
            [[nodiscard]]
            net::INetworkEngine *getCurrentNetworkEngine(void) const;

            /**
             * @brief Get the current rendering engine
             * @return Pointer to the active IRenderer instance
             */
            [[nodiscard]]
            render::IRenderer *getCurrentRendereEngine(void) const;

            using UpdateCallback = std::function<void(float)>;
            using RenderCallback = std::function<void(render::RenderFrame&)>;
            using EventCallback = std::function<void(const input::Event&)>;

            void setUpdateCallback(UpdateCallback callback);
            void setRenderCallback(RenderCallback callback);
            void setEventCallback(EventCallback callback);

            [[nodiscard]]
            const Config& config(void) const;

        private:
            Core() = default;

            /**
             * @brief Change the active rendering engine at runtime
             * @param path Path to the new rendering engine plugin
             */
            void changeRenderEngine(const std::string& path);

            /**
             * @brief Load configuration from a file
             * @param path Path to the configuration file
             */
            void loadConfiguration(const std::string& path);

        private:
            Config _config{};
            render::RenderFrame _frame{};

            plugin::LibraryManager _libraries;
            std::shared_ptr<plugin::DynamicLibrary> _rendererLoader{};
            render::IRenderer* _activeRendererEngine{nullptr};
            std::shared_ptr<plugin::DynamicLibrary> _networkLoader{};
            net::INetworkEngine* _activeNetworkEngine{nullptr};
            bool _running{false};
            bool _initialized{false};

            std::set<std::string> _loadedPlugins{};
            std::vector<std::string> _availableRenderers{};
            std::vector<std::string> _availableNetworkEngines{};

            UpdateCallback _updateFn{};
            RenderCallback _renderFn{};
            EventCallback _eventFn{};
    };
}

#endif /* !RTYPE_CORE_HPP_ */
