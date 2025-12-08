/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Registry.cpp
*/

/**
 * @file Registry.cpp
 * @brief Registry class implementation
 */
#include "RType/Assert.hpp"
#include "RType/ECS/Registry.hpp"

namespace rtp::ecs
{
    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    auto Registry::spawnEntity(void) -> std::expected<Entity, std::string>
    {
        std::lock_guard<std::mutex> lock(this->_mutex);

        if (!this->_freeIndices.empty()) {
            std::uint32_t idx = this->_freeIndices.front();
            this->_freeIndices.pop_front();

            return Entity(idx, this->_generations[idx]);
        }

        if (this->_generations.size() >= Entity::MAX_INDEX)
            return std::unexpected("Registry: Max entities reached, cannot spawn new entity.");

        std::uint32_t idx = static_cast<std::uint32_t>(this->_generations.size());

        this->_generations.push_back(0); 

        return Entity(idx, 0);
    }

    void Registry::killEntity(Entity entity)
    {
        std::lock_guard<std::mutex> lock(this->_mutex);

        std::uint32_t idx = entity.index();

        if (idx >= this->_generations.size() ||
            this->_generations[idx] != entity.generation())
            return;

        for (auto &pair : this->_arrays)
            pair.second->erase(entity);

        this->_generations[idx]++;

        this->_freeIndices.push_back(idx);
    }
}
