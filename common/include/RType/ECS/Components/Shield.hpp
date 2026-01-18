/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Shield component
*/

#pragma once

namespace rtp::ecs::components {

/**
 * @struct Shield
 * @brief Component representing a shield that absorbs damage
 */
struct Shield {
    int charges{1}; /**< Number of hits the shield can absorb */
};

}  // namespace rtp::ecs::components
