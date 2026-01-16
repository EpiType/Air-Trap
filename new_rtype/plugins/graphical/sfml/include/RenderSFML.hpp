/**
 * File   : SFML.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#ifndef ENGINE_RENDER_SFML_HPP_
    #define ENGINE_RENDER_SFML_HPP_

    #include "engine/render/IRenderer.hpp"
    #include "engine/core/Logger.hpp"
    #include <SFML/Graphics.hpp>
    #include <unordered_map>

namespace engine::render
{
    /**
     * @class RenderSFML
     * @brief SFML implementation of the IRenderer interface
     * @details This class provides rendering capabilities using the SFML
     * library. It implements the IRenderer interface defined in the engine
     * core, allowing for window creation, frame management, and drawing
     * operations.
     */
    class RenderSFML : public IRenderer
    {
        public:
            RenderSFML() = default;
            
            bool init(int width, int height, const std::string& title) override;

            void shutdown(void) override;

            std::string getName(void) const override;

            void beginFrame(void) override;

            void draw(const RenderFrame& frame) override;

            void endFrame(void) override;

            void resize(int width, int height) override;

            std::uint32_t loadTexture(const std::string& path) override;

            void unloadTexture(std::uint32_t id) override;

            std::uint32_t loadFont(const std::string& path) override;

            void unloadFont(std::uint32_t id) override;

            bool pollEvents(std::vector<input::Event>& outEvents) override;

        private:
            std::string _name{"SFML"};

            sf::RenderWindow _window;

            std::unordered_map<std::uint32_t, sf::Texture> _textures;
            std::unordered_map<std::uint32_t, sf::Font> _fonts;
            std::uint32_t _nextTextureId{1};
            std::uint32_t _nextFontId{1};
    };
} // namespace engine::render

extern "C" {
    /**
     * @brief Factory function to create an instance of the SFML renderer
     * @return Pointer to the created IRenderer instance
     */
    engine::render::IRenderer* CreateRenderer();

    /**
     * @brief Function to destroy an instance of the SFML renderer
     * @param renderer Pointer to the IRenderer instance to be destroyed
     */
    void DestroyRenderer(engine::render::IRenderer* renderer);
}

#endif // ENGINE_RENDER_SFML_HPP_

