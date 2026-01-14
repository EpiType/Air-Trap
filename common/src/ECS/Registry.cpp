/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Registry.cpp
*/

/*
** MIT License
**
** Copyright (c) 2025 Robin Toillon
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**
 * @file Registry.cpp
 * @brief Registry class implementation
 * @author Robin Toillon
 * @details Implements entity lifecycle management including spawning,
 * killing entities, and managing entity generations for safe recycling
 * of entity IDs.
 */

#include "RType/Assert.hpp"
#include "RType/ECS/Registry.hpp"

namespace rtp::ecs
{
    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    auto Registry::spawn(void) -> std::expected<Entity, rtp::Error>
    {
        std::unique_lock lock(this->_mutex);

        if (!this->_freeIndices.empty()) {
            std::size_t idx = this->_freeIndices.front();
            this->_freeIndices.pop_front();

            return Entity(idx, this->_generations[idx]);
        }

        if (this->_generations.size() >= Entity::MAX_INDEX)
            return std::unexpected{Error::failure(ErrorCode::RegistryFull,
                "Registry: Max entities reached, cannot spawn new entity.")};

        std::size_t idx = this->_generations.size();

        this->_generations.push_back(0); 

        return Entity(idx, 0);
    }

    void Registry::kill(Entity entity)
    {
        std::unique_lock lock(this->_mutex);

        std::uint32_t idx = entity.index();

        if (idx >= this->_generations.size() ||
            this->_generations[idx] != entity.generation())
            return;

        for (auto &pair : this->_arrays)
            pair.second->erase(entity);

        this->_generations[idx]++;
        this->_freeIndices.push_back(idx);
    }

    bool Registry::isAlive(Entity entity) const noexcept
    {
        std::shared_lock lock(this->_mutex);

        std::uint32_t idx = entity.index();
        
        if (idx >= this->_generations.size())
            return false;
            
        return this->_generations[idx] == entity.generation();
    }

    void Registry::clear(void) noexcept
    {
        std::unique_lock lock(this->_mutex);

        for (auto &pair : this->_arrays)
            pair.second->clear();

        this->_generations.clear();
        this->_freeIndices.clear();
    }

    void Registry::purge(void) noexcept
    {
        std::unique_lock lock(this->_mutex);

        for (auto &pair : this->_arrays)
            pair.second->clear();

        this->_freeIndices.clear();

        auto idxs = std::views::iota(0uz, this->_generations.size())
                  | std::views::filter([this](std::size_t i) { return this->_generations[i] != 0; });
        std::ranges::copy(idxs, std::back_inserter(this->_freeIndices));
    }

    std::size_t Registry::entityCount(void) const noexcept
    {
        std::shared_lock lock(this->_mutex);
        return this->_generations.size() - this->_freeIndices.size();
    }
}
