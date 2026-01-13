/**
 * File   : Ammo.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_ECS_COMPONENTS_AMMO_HPP_
#define RTYPE_ECS_COMPONENTS_AMMO_HPP_

#include <cstdint>

namespace rtp::ecs::components {
/**
 * @struct Ammo
 * @brief Ammo tracking for weapons.
 */
struct Ammo {
    uint16_t current{100};
    uint16_t max{100};
    float reloadCooldown{2.0f};
    float reloadTimer{0.0f};
    bool isReloading{false};
    bool dirty{true};
};
} // namespace rtp::ecs::components

#endif /* !RTYPE_ECS_COMPONENTS_AMMO_HPP_ */
