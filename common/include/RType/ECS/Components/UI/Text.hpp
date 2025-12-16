/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Text
*/

#pragma once

#include <string>
#include "RType/Math/Vec2.hpp"

namespace rtp::ecs::components::ui {

/**
 * @struct Text
 * @brief Component representing text to render
 */
struct Text {
    std::string content;          /**< Text content */
    Vec2f position;               /**< Position of the text */
    std::string fontPath;         /**< Path to the font file */
    unsigned int fontSize{24};    /**< Font size */
    uint8_t red{255};             /**< Red component */
    uint8_t green{255};           /**< Green component */
    uint8_t blue{255};            /**< Blue component */
    uint8_t alpha{255};           /**< Alpha (opacity) */
    int zIndex{0};                /**< Rendering order */
};

}  // namespace rtp::ecs::components::ui