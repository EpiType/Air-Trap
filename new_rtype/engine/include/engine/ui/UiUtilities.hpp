/**
 * File   : UiFactory.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#ifndef RTYPE_CLIENT_GRAPHICS_UIFACTORY_HPP_
    #define RTYPE_CLIENT_GRAPHICS_UIFACTORY_HPP_

    #include "engine/ecs/Registry.hpp"
    #include "engine/math/Vec2.hpp"
    #include <cstdint>
    #include <optional>
    #include <string>

namespace aer::ui
{
    ////////////////////////////////////////////////////////////////////////////
    // Structs
    ////////////////////////////////////////////////////////////////////////////

    struct color {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        std::optional<uint8_t> a{255};
    };

    struct textContainer {
        std::string content;
        std::string FontPath;
        unsigned int fontSize;
    };

    using position = aer::math::Vec2f;

    using size = aer::math::Vec2f;

}

#endif /* !RTYPE_CLIENT_GRAPHICS_UIFACTORY_HPP_ */
