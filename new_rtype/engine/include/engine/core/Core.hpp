/**
 * File   : Core.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#ifndef RTYPE_CORE_HPP_
    #define RTYPE_CORE_HPP_

    #include "engine/core/LogLevel.hpp"
    #include "engine/core/Logger.hpp"
    #include "engine/core/Time.hpp"
    #include "engine/core/Error.hpp"
    #include "engine/plugin/LibraryManager.hpp"
    #include "engine/render/IRenderer.hpp"
    #include "engine/render/RenderFrame.hpp"

    #include <functional>
    #include <memory>
    #include <string>

namespace engine::core
{
    class Core {
        public:
            struct Config {
                std::string rendererPluginPath{};
                int width{1280};
                int height{720};
                std::string title{"Engine"};
                double fixedStep{1.0 / 60.0};
                bool useFixedStep{true};
            };

            using UpdateFn = std::function<void(float)>;
            using RenderFn = std::function<void(render::RenderFrame&)>;

            static Core& instance();

            bool init(const Config& config);
            void run();
            void stop();
            void shutdown();

            void setUpdateCallback(UpdateFn fn);
            void setRenderCallback(RenderFn fn);

            plugin::LibraryManager& libraries();

            [[nodiscard]]
            auto loadLibrary(const std::string& path)
                -> std::expected<std::shared_ptr<plugin::DynamicLibrary>, core::Error>;

            [[nodiscard]]
            bool loadRendererPlugin(const std::string& path);

        private:
            Core() = default;

            bool loadRendererPluginInternal();

        private:
            Config _config{};
            UpdateFn _updateFn{};
            RenderFn _renderFn{};
            render::RenderFrame _frame{};

            plugin::LibraryManager _libraries;
            std::shared_ptr<plugin::DynamicLibrary> _rendererLib{};
            render::IRenderer* _renderer{nullptr};
            bool _running{false};
            bool _initialized{false};
    };
}

#endif /* !RTYPE_CORE_HPP_ */
