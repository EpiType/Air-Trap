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
#include <filesystem>
#include <fstream>
#include <string_view>
#include <utility>
#include <vector>

namespace aer::core
{
    namespace
    {
        using CreateRendererFn = render::IRenderer* (*)();
        using DestroyRendererFn = void (*)(render::IRenderer*);
        using CreateNetworkEngineFn = net::INetworkEngine* (*)();
        using DestroyNetworkEngineFn = void (*)(net::INetworkEngine*);

        std::string_view trimValue(std::string_view input)
        {
            while (!input.empty() && std::isspace(static_cast<unsigned char>(input.front()))) {
                input.remove_prefix(1);
            }
            while (!input.empty() && std::isspace(static_cast<unsigned char>(input.back()))) {
                input.remove_suffix(1);
            }
            return input;
        }
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
            aer::log::error("Exception while loading configuration: {}", e.what());
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
        if (config.width > 0) {
            _config.width = config.width;
        }
        if (config.height > 0) {
            _config.height = config.height;
        }
        _config.headless = config.headless;

        if (_config.headless) {
            _initialized = true;
            return true;
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
        -> std::expected<std::shared_ptr<plugin::DynamicLibrary>, log::Error>
    {
        return _libraries.loadShared(path);
    }

    bool Core::loadRendererPlugin(const std::string& path)
    {
        if (path.empty()) return false;

        auto libRes = _libraries.loadShared(_config.rendererPluginPath);
        if (!libRes) {
            aer::log::error("Renderer plugin load failed: {}", libRes.error());
            return false;
        }

        _rendererLoader = libRes.value();

        auto createRes = _rendererLoader->get<CreateRendererFn>("CreateRenderer");
        if (!createRes) {
            aer::log::error("Renderer CreateRenderer not found: {}", createRes.error());
            return false;
        }

        _activeRendererEngine = createRes.value()();
        if (!_activeRendererEngine) {
            aer::log::error("Renderer CreateRenderer returned null");
            return false;
        }

        _config.rendererPluginPath = path;

        return _activeRendererEngine->init(_config.width, _config.height, _config.title);
    }

    bool Core::loadNetworkPlugin(const std::string& path)
    {
        if (path.empty()) return false;

        auto libRes = _libraries.loadShared(_config.networkPluginPath);
        if (!libRes) {
            aer::log::error("Network plugin load failed: {}", libRes.error());
            return false;
        }

        _networkLoader = libRes.value();

        auto createRes = _networkLoader->get<CreateNetworkEngineFn>("CreateNetworkEngine");
        if (!createRes) {
            aer::log::error("Network CreateNetworkEngine not found: {}", createRes.error());
            return false;
        }

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
            aer::log::error("Failed to open configuration file: {}, impossible to launch engine without any configuration file.", path);
            return;
        }

        aer::log::info("=============== Engine Configuration ===============", path);
        aer::log::info("Loading configuration from file: {}", path);
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
            aer::log::info("Available Renderers:");
            for (const auto &renderer : _config.availableRenderers) {
                aer::log::info(" - {}", renderer);
            }
        }
        if (std::size_t pos = findValueStart("availableNetworkEngines");
            pos != std::string_view::npos) {
            parseStringArray(pos, _config.availableNetworkEngines);
            aer::log::info("Available Network Engines:");
            for (const auto &network : _config.availableNetworkEngines) {
                aer::log::info(" - {}", network);
            }
        }
        if (std::size_t pos = findValueStart("defaultRenderPlugin");
            pos != std::string_view::npos) {
            parseString(pos, _config.defaultRendererPluginPath);
            aer::log::info("Default Renderer Plugin Path: {}",
                             _config.defaultRendererPluginPath);
            _config.rendererPluginPath = _config.defaultRendererPluginPath;
        }
        if (std::size_t pos = findValueStart("defaultNetworkPlugin");
            pos != std::string_view::npos) {
            parseString(pos, _config.defaultNetworkPluginPath);
            aer::log::info("Default Network Plugin Path: {}",
                             _config.defaultNetworkPluginPath);
            _config.networkPluginPath = _config.defaultNetworkPluginPath;
        }
        if (std::size_t pos = findValueStart("defaultGameClient");
            pos != std::string_view::npos) {
            parseString(pos, _config.defaultGameClientPath);
            aer::log::info("Default Game Client Path: {}",
                             _config.defaultGameClientPath);
        }
        if (std::size_t pos = findValueStart("defaultGameServer");
            pos != std::string_view::npos) {
            parseString(pos, _config.defaultGameServerPath);
            aer::log::info("Default Game Server Path: {}",
                             _config.defaultGameServerPath);
        }
        if (std::size_t pos = findValueStart("title");
            pos != std::string_view::npos) {
            parseString(pos, _config.title);
            aer::log::info("Window Title: {}", _config.title);
        }
        if (std::size_t pos = findValueStart("assetsPath");
            pos != std::string_view::npos) {
            parseString(pos, _config.assetsPath);
            aer::log::info("Assets Path: {}", _config.assetsPath);
        }
        if (std::size_t pos = findValueStart("networkIp");
            pos != std::string_view::npos) {
            parseString(pos, _config.networkIp);
            aer::log::info("Network IP: {}", _config.networkIp);
        }
        if (std::size_t pos = findValueStart("width");
            pos != std::string_view::npos) {
            int value = _config.width;
            if (parseInt(pos, value)) {
                _config.width = value;
            }
            aer::log::info("Window Width: {}", _config.width);
        }
        if (std::size_t pos = findValueStart("height");
            pos != std::string_view::npos) {
            int value = _config.height;
            if (parseInt(pos, value)) {
                _config.height = value;
            }
            aer::log::info("Window Height : {}", _config.height);
        }
        if (std::size_t pos = findValueStart("networkPort");
            pos != std::string_view::npos) {
            int value = _config.networkPort;
            if (parseInt(pos, value)) {
                _config.networkPort = static_cast<std::uint16_t>(value);
            }
            aer::log::info("Network Port: {}", _config.networkPort);
        }

        if (!_config.defaultGameClientPath.empty()) {
            namespace fs = std::filesystem;
            fs::path clientPath{_config.defaultGameClientPath};
            fs::path baseDir = clientPath.has_parent_path()
                ? clientPath.parent_path()
                : fs::path{};
            const fs::path ymlPath = baseDir / "config.yml";
            const fs::path yamlPath = baseDir / "config.yaml";
            if (fs::exists(ymlPath)) {
                loadGameConfiguration(ymlPath.string());
            } else if (fs::exists(yamlPath)) {
                loadGameConfiguration(yamlPath.string());
            } else {
                aer::log::warning("Game config not found next to default game client: {}",
                                   _config.defaultGameClientPath);
            }
        }

        aer::log::info("====================================================");
    }

    void Core::loadGameConfiguration(const std::string& path)
    {
        std::ifstream file(path);
        if (!file) {
            aer::log::warning("Failed to open game configuration file: {}", path);
            return;
        }

        bool inWindow = false;
        bool applied = false;
        std::string line;
        while (std::getline(file, line)) {
            std::string_view view(line);
            const auto hash = view.find('#');
            if (hash != std::string_view::npos) {
                view = view.substr(0, hash);
            }
            view = trimValue(view);
            if (view.empty()) continue;

            if (view == "window:" || view == "window") {
                inWindow = true;
                continue;
            }
            if (view.back() == ':') {
                inWindow = false;
                continue;
            }
            if (!inWindow) {
                continue;
            }

            const auto colon = view.find(':');
            if (colon == std::string_view::npos) continue;
            auto key = trimValue(view.substr(0, colon));
            auto value = trimValue(view.substr(colon + 1));

            if (!value.empty() && value.front() == '"' && value.back() == '"' && value.size() >= 2) {
                value = value.substr(1, value.size() - 2);
            }

            if (key == "width") {
                int parsed = _config.width;
                auto res = std::from_chars(value.data(), value.data() + value.size(), parsed);
                if (res.ec == std::errc()) {
                    _config.width = parsed;
                    applied = true;
                }
            } else if (key == "height") {
                int parsed = _config.height;
                auto res = std::from_chars(value.data(), value.data() + value.size(), parsed);
                if (res.ec == std::errc()) {
                    _config.height = parsed;
                    applied = true;
                }
            } else if (key == "title") {
                _config.title.assign(value);
                applied = true;
            }
        }

        if (applied) {
            aer::log::info("Game config applied: window {}x{} '{}'",
                           _config.width, _config.height, _config.title);
        }
    }
}
