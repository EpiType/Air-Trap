/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Component
*/

#pragma once
#include <bitset>
#include <cstdint>

namespace rtp::ecs {
using Component = std::uint8_t;
const Component MAX_COMPONENTS = 32;

using Signature = std::bitset<MAX_COMPONENTS>;
}  // namespace rtp::ecs