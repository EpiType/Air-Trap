/**
 * File   : Core.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#include "engine/core/Core.hpp"
#include "engine/input/Input.hpp"

#include <chrono>
#include <utility>
#include <vector>

namespace engine::core
{
    namespace
    {
        using CreateRendererFn = render::IRenderer* (*)();
        using DestroyRendererFn = void (*)(render::IRenderer*);
    }

    Core& Core::instance()
    {
        static Core core;
        return core;
    }

    bool Core::init(const Config& config)
    {
        if (_initialized) {
            return true;
        }

        _config = config;
        if (!loadRendererPluginInternal()) {
            return false;
        }

        _initialized = (_renderer != nullptr);
        return _initialized;
    }

    void Core::run()
    {
        if (!_initialized || !_renderer) {
            return;
        }

        _running = true;
        std::vector<input::Event> events;
        auto &inputManager = input::Input::instance();
        using clock = std::chrono::steady_clock;
        auto last = clock::now();
        double accumulator = 0.0;

        while (_running) {
            auto now = clock::now();
            const double dt = std::chrono::duration<double>(now - last).count();
            last = now;

            if (!_renderer->pollEvents(events)) {
                stop();
            }
            inputManager.processEvents(events);
            if (inputManager.closeRequested()) {
                stop();
            }

            if (_updateFn) {
                if (_config.useFixedStep) {
                    accumulator += dt;
                    const double step = _config.fixedStep;
                    while (accumulator >= step) {
                        _updateFn(static_cast<float>(step));
                        accumulator -= step;
                    }
                } else {
                    _updateFn(static_cast<float>(dt));
                }
            }

            _renderer->beginFrame();
            _frame.clear();
            if (_renderFn) {
                _renderFn(_frame);
                _renderer->draw(_frame);
            }
            _renderer->endFrame();
        }
    }

    void Core::stop()
    {
        _renderer->endFrame();
        _renderer->shutdown();
        _running = false;
    }

    void Core::shutdown()
    {
        if (_rendererLib && _renderer) {
            auto destroyRes = _rendererLib->get<DestroyRendererFn>("DestroyRenderer");
            if (destroyRes) {
                destroyRes.value()(_renderer);
            }
        }
        _renderer = nullptr;
        _rendererLib.reset();
        _initialized = false;
    }

    void Core::setUpdateCallback(UpdateFn fn)
    {
        _updateFn = std::move(fn);
    }

    void Core::setRenderCallback(RenderFn fn)
    {
        _renderFn = std::move(fn);
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
        _config.rendererPluginPath = path;
        return loadRendererPluginInternal();
    }

    bool Core::loadRendererPluginInternal()
    {
        if (_config.rendererPluginPath.empty()) {
            return false;
        }

        auto libRes = _libraries.loadShared(_config.rendererPluginPath);
        if (!libRes) {
            return false;
        }

        _rendererLib = libRes.value();

        auto createRes = _rendererLib->get<CreateRendererFn>("CreateRenderer");
        if (!createRes) {
            return false;
        }

        _renderer = createRes.value()();
        if (!_renderer) {
            return false;
        }

        return _renderer->init(_config.width, _config.height, _config.title);
    }
}
