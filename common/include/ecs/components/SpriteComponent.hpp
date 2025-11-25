#pragma once

#include <string>

namespace ecs {

struct SpriteComponent {
    std::string texturePath;
    int width;
    int height;
    int layer; // Z-order for rendering
};

} // namespace ecs
