/**
 * File   : RenderSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/systems/RenderSystem.hpp"

namespace rtp::client::systems
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    RenderSystem::RenderSystem(engine::ecs::Registry& registry,
                               engine::render::IRenderer& renderer,
                               engine::render::RenderFrame& frame)
        : _registry(registry), _renderer(renderer), _frame(frame)
    {
    }

    void RenderSystem::update(float)
    {
        _frame.sprites.clear();

        auto view = _registry.zipView<engine::ecs::components::Transform,
                                      engine::ecs::components::Sprite>();

        for (auto&& [transform, sprite] : view) {
            engine::render::RenderSprite renderSprite;
            renderSprite.textureId = getTextureId(sprite.texturePath);
            renderSprite.position = { transform.position.x, transform.position.y };
            renderSprite.scale = { transform.scale.x, transform.scale.y };
            renderSprite.rotation = transform.rotation;
            renderSprite.origin = { 0.0f, 0.0f };
            renderSprite.srcOffset = { static_cast<float>(sprite.rectLeft),
                                       static_cast<float>(sprite.rectTop) };
            renderSprite.srcSize = { static_cast<float>(sprite.rectWidth),
                                     static_cast<float>(sprite.rectHeight) };
            renderSprite.r = static_cast<float>(sprite.red) / 255.0f;
            renderSprite.g = static_cast<float>(sprite.green) / 255.0f;
            renderSprite.b = static_cast<float>(sprite.blue) / 255.0f;
            renderSprite.a = static_cast<float>(sprite.opacity) / 255.0f;
            renderSprite.z = sprite.zIndex;

            _frame.sprites.push_back(renderSprite);
        }
    }

    //////////////////////////////////////////////////////////////////////////
    // Private API
    //////////////////////////////////////////////////////////////////////////

    std::uint32_t RenderSystem::getTextureId(const std::string& path)
    {
        const auto it = _textureCache.find(path);
        if (it != _textureCache.end()) {
            return it->second;
        }
        const auto id = _renderer.loadTexture(path);
        _textureCache.emplace(path, id);
        return id;
    }
} // namespace rtp::client::systems
