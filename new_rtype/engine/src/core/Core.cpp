/**
 * File   : Core.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#include "engine/core/Core.hpp"
#include "engine/input/Input.hpp"

#include <chrono>
#include <cctype>
#include <charconv>
#include <fstream>
#include <string_view>
#include <utility>
#include <vector>

namespace engine::core
{
    namespace
    {
        using CreateRendererFn = render::IRenderer* (*)();
        using DestroyRendererFn = void (*)(render::IRenderer*);
        using CreateNetworkEngineFn = net::INetworkEngine* (*)();
        using DestroyNetworkEngineFn = void (*)(net::INetworkEngine*);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    Core& Core::instance()
    {
        static Core core;
        return core;
    }

    bool Core::init(const Config& config)
    {
        if (_initialized) return true;

        try {
            loadConfiguration("./engine/config.json");
        } catch (const std::exception& e) {
            engine::core::error("Exception while loading configuration: {}", e.what());
            return false;
        }

        if (!config.rendererPluginPath.empty()) {
            _config.rendererPluginPath = config.rendererPluginPath;
        }
        if (!config.networkPluginPath.empty()) {
            _config.networkPluginPath = config.networkPluginPath;
        }
        if (!config.title.empty()) {
            _config.title = config.title;
        }

        if (!loadRendererPlugin(_config.rendererPluginPath)) {
            return false;
        }

        _initialized = (_activeRendererEngine != nullptr);
        return _initialized;
    }

    void Core::run()
    {
        if (!_initialized || !_activeRendererEngine) return;

        _running = true;
        auto &inputManager = input::Input::instance();
        using clock = std::chrono::steady_clock;
        auto last = clock::now();

        while (_running)
        {
            const auto now = clock::now();
            const float dt = std::chrono::duration<float>(now - last).count();
            last = now;

            if (!_activeRendererEngine) {
                _running = false;
                break;
            }
            if (_activeRendererEngine->hasFocus()) {
                const auto events = _activeRendererEngine->pollEvents();
                inputManager.processEvents(events);
                if (_eventFn) {
                    for (const auto &event : events) {
                        _eventFn(event);
                    }
                }
                if (inputManager.closeRequested()) {
                    stop();
                    break;
                }
            }

            if (_updateFn) {
                _updateFn(dt);
            }

            _activeRendererEngine->beginFrame();
            _frame.clear();
            if (_renderFn) {
                _renderFn(_frame);
            }
            _activeRendererEngine->draw(_frame);
            _activeRendererEngine->endFrame();
        }
    }

    void Core::stop()
    {
        _activeRendererEngine->endFrame();
        _activeRendererEngine->shutdown();
        _running = false;
    }

    void Core::shutdown()
    {
        if (_rendererLoader && _activeRendererEngine) {
            auto destroyRes = _rendererLoader->get<DestroyRendererFn>("DestroyRenderer");
            if (destroyRes) {
                destroyRes.value()(_activeRendererEngine);
            }
        }
        _activeRendererEngine = nullptr;
        _rendererLoader.reset();
        if (_networkLoader && _activeNetworkEngine) {
            auto destroyRes = _networkLoader->get<DestroyNetworkEngineFn>("DestroyNetworkEngine");
            if (destroyRes) {
                destroyRes.value()(_activeNetworkEngine);
            }
        }
        _activeNetworkEngine = nullptr;
        _networkLoader.reset();
        _initialized = false;
    }

    plugin::LibraryManager& Core::libraries()
    {
        return _libraries;
    }

    auto Core::loadLibrary(const std::string& path)
        -> std::expected<std::shared_ptr<plugin::DynamicLibrary>, core::Error>
    {
        return _libraries.loadShared(path);
    }

    bool Core::loadRendererPlugin(const std::string& path)
    {
        if (path.empty()) return false;

        auto libRes = _libraries.loadShared(_config.rendererPluginPath);
        if (!libRes) return false;

        _rendererLoader = libRes.value();

        auto createRes = _rendererLoader->get<CreateRendererFn>("CreateRenderer");
        if (!createRes) return false;

        _activeRendererEngine = createRes.value()();
        if (!_activeRendererEngine) return false;

        _config.rendererPluginPath = path;

        return _activeRendererEngine->init(_config.width, _config.height, _config.title);
    }

    bool Core::loadNetworkPlugin(const std::string& path)
    {
        if (path.empty()) return false;

        auto libRes = _libraries.loadShared(_config.networkPluginPath);
        if (!libRes) return false;

        _networkLoader = libRes.value();

        auto createRes = _networkLoader->get<CreateNetworkEngineFn>("CreateNetworkEngine");
        if (!createRes) return false;

        _config.networkPluginPath = path;

        _activeNetworkEngine = createRes.value()();
        return _activeNetworkEngine != nullptr;
    }

    net::INetworkEngine *Core::getCurrentNetworkEngine(void) const
    {
        return _activeNetworkEngine;
    }

    render::IRenderer *Core::getCurrentRendereEngine(void) const
    {
        return _activeRendererEngine;
    }

    void Core::setUpdateCallback(UpdateCallback callback)
    {
        _updateFn = std::move(callback);
    }

    void Core::setRenderCallback(RenderCallback callback)
    {
        _renderFn = std::move(callback);
    }

    void Core::setEventCallback(EventCallback callback)
    {
        _eventFn = std::move(callback);
    }

    const Core::Config& Core::config(void) const
    {
        return _config;
    }

    ///////////////////////////////////////////////////////////////////////////////
    // Private API
    ///////////////////////////////////////////////////////////////////////////////

    void Core::changeRenderEngine(const std::string& path)
    {
        if (path.empty() || path == _config.rendererPluginPath) return;

        if (_rendererLoader && _activeRendererEngine) {
            auto destroyRes = _rendererLoader->get<DestroyRendererFn>("DestroyRenderer");
            if (destroyRes) {
                destroyRes.value()(_activeRendererEngine);
            }
        }
        _activeRendererEngine = nullptr;
        _rendererLoader.reset();

        if (!loadRendererPlugin(path)) {
            return;
        }
    }

    void Core::loadConfiguration(const std::string& path)
    {
        std::ifstream file(path, std::ios::binary);
        if (!file) {
            engine::core::error("Failed to open configuration file: {}, impossible to launch engine without any configuration file.", path);
            return;
        }

        engine::core::info("=============== Engine Configuration ===============", path);
        engine::core::info("Loading configuration from file: {}", path);
        file.seekg(0, std::ios::end);
        const std::streamsize size = file.tellg();
        if (size <= 0) return;
        std::string content(static_cast<std::size_t>(size), '\0');
        file.seekg(0, std::ios::beg);
        file.read(content.data(), size);
        if (!file) return;

        const std::string_view view(content);

        const auto skipWs = [&](std::size_t i) {
            while (i < view.size()
                   && std::isspace(static_cast<unsigned char>(view[i]))) {
                ++i;
            }
            return i;
        };

        const auto findValueStart = [&](std::string_view key) -> std::size_t {
            std::string needle;
            needle.reserve(key.size() + 2);
            needle.push_back('"');
            needle.append(key);
            needle.push_back('"');
            std::size_t pos = view.find(needle);
            if (pos == std::string_view::npos) return pos;
            pos = view.find(':', pos + needle.size());
            if (pos == std::string_view::npos) return pos;
            return skipWs(pos + 1);
        };

        const auto parseString = [&](std::size_t &i, std::string &out) {
            i = skipWs(i);
            if (i >= view.size() || view[i] != '"') return false;
            ++i;
            const std::size_t start = i;
            while (i < view.size() && view[i] != '"') {
                if (view[i] == '\\' && i + 1 < view.size()) {
                    i += 2;
                    continue;
                }
                ++i;
            }
            if (i >= view.size()) return false;
            out.assign(view.substr(start, i - start));
            ++i;
            return true;
        };

        const auto parseStringArray = [&](std::size_t i, std::vector<std::string> &out) {
            i = skipWs(i);
            if (i >= view.size() || view[i] != '[') return false;
            ++i;
            out.clear();
            while (i < view.size()) {
                i = skipWs(i);
                if (i < view.size() && view[i] == ']') {
                    ++i;
                    return true;
                }
                if (i < view.size() && view[i] == ',') {
                    ++i;
                    continue;
                }
                std::string item;
                if (!parseString(i, item)) return false;
                out.push_back(std::move(item));
                i = skipWs(i);
                if (i < view.size() && view[i] == ',') {
                    ++i;
                }
            }
            return false;
        };

        const auto parseInt = [&](std::size_t i, int &out) {
            i = skipWs(i);
            const char *begin = view.data() + i;
            const char *end = view.data() + view.size();
            const auto res = std::from_chars(begin, end, out);
            if (res.ec != std::errc()) return false;
            return true;
        };

        if (std::size_t pos = findValueStart("availableRendererEngines");
            pos != std::string_view::npos) {
            parseStringArray(pos, _config.availableRenderers);
            engine::core::info("Available Renderers:");
            for (const auto &renderer : _config.availableRenderers) {
                engine::core::info(" - {}", renderer);
            }
        }
        if (std::size_t pos = findValueStart("availableNetworkEngines");
            pos != std::string_view::npos) {
            parseStringArray(pos, _config.availableNetworkEngines);
            engine::core::info("Available Network Engines:");
            for (const auto &network : _config.availableNetworkEngines) {
                engine::core::info(" - {}", network);
            }
        }
        if (std::size_t pos = findValueStart("defaultRenderPlugin");
            pos != std::string_view::npos) {
            parseString(pos, _config.defaultRendererPluginPath);
            engine::core::info("Default Renderer Plugin Path: {}",
                             _config.defaultRendererPluginPath);
            _config.rendererPluginPath = _config.defaultRendererPluginPath;
        }
        if (std::size_t pos = findValueStart("defaultNetworkPlugin");
            pos != std::string_view::npos) {
            parseString(pos, _config.defaultNetworkPluginPath);
            engine::core::info("Default Network Plugin Path: {}",
                             _config.defaultNetworkPluginPath);
            _config.networkPluginPath = _config.defaultNetworkPluginPath;
        }
        if (std::size_t pos = findValueStart("title");
            pos != std::string_view::npos) {
            parseString(pos, _config.title);
            engine::core::info("Window Title: {}", _config.title);
        }
        if (std::size_t pos = findValueStart("assetsPath");
            pos != std::string_view::npos) {
            parseString(pos, _config.assetsPath);
            engine::core::info("Assets Path: {}", _config.assetsPath);
        }
        if (std::size_t pos = findValueStart("networkIp");
            pos != std::string_view::npos) {
            parseString(pos, _config.networkIp);
            engine::core::info("Network IP: {}", _config.networkIp);
        }
        if (std::size_t pos = findValueStart("width");
            pos != std::string_view::npos) {
            int value = _config.width;
            if (parseInt(pos, value)) {
                _config.width = value;
            }
            engine::core::info("Window Width: {}", _config.width);
        }
        if (std::size_t pos = findValueStart("height");
            pos != std::string_view::npos) {
            int value = _config.height;
            if (parseInt(pos, value)) {
                _config.height = value;
            }
            engine::core::info("Window Height : {}", _config.height);
        }
        if (std::size_t pos = findValueStart("networkPort");
            pos != std::string_view::npos) {
            int value = _config.networkPort;
            if (parseInt(pos, value)) {
                _config.networkPort = static_cast<std::uint16_t>(value);
            }
            engine::core::info("Network Port: {}", _config.networkPort);
        }
        engine::core::info("====================================================");
    }
}
