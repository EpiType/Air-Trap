/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Types
*/

#pragma once
#include <bitset>
#include <cstdint>

namespace rtp::ecs {
/**
 * @typedef Entity
 * @brief Type representing an entity ID.
 */
using Entity = std::uint32_t;
const Entity MAX_ENTITIES = 5000;

/**
 * @typedef Component
 * @brief Type representing a component type ID.
 */
using Component = std::uint8_t;
const Component MAX_COMPONENTS = 32;

/**
 * @typedef Signature
 * @brief Type representing a component signature for an entity.
 */
using Signature = std::bitset<MAX_COMPONENTS>;
}  // namespace rtp::ecs