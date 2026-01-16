/**
 * File   : main.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#include "engine/core/Core.hpp"

#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <string>

namespace
{
    std::string defaultRendererPath()
    {
#if defined(_WIN32)
        return "runtime/plugins/render_sfml.dll";
#elif defined(__APPLE__)
        return "runtime/plugins/librender_sfml.dylib";
#else
        return "runtime/plugins/librender_sfml.so";
#endif
    }

    std::string resolveRendererPath(const char *argPath)
    {
        if (argPath && *argPath != '\0') {
            return argPath;
        }

        const std::string primary = defaultRendererPath();
        if (std::filesystem::exists(primary)) {
            return primary;
        }

        std::filesystem::path fallback = std::filesystem::path("plugins")
#if defined(_WIN32)
            / "render_sfml.dll";
#elif defined(__APPLE__)
            / "librender_sfml.dylib";
#else
            / "librender_sfml.so";
#endif
        return fallback.string();
    }
}

int main(int argc, char **argv)
{
    auto &core = engine::core::Core::instance();
    engine::core::Core::Config config{};

    config.title = "RType Engine";
    config.rendererPluginPath = resolveRendererPath((argc > 1) ? argv[1] : nullptr);

    if (!core.init(config)) {
        std::fprintf(stderr,
                     "Failed to init engine (renderer: %s)\n",
                     config.rendererPluginPath.c_str());
        return EXIT_FAILURE;
    }

    core.setUpdateCallback([](float) {});
    core.setRenderCallback([](engine::render::RenderFrame &) {});

    core.run();
    core.shutdown();

    return EXIT_SUCCESS;
}
