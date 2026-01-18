/**
 * File   : SFML.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#ifndef ENGINE_RENDER_SFML_HPP_
    #define ENGINE_RENDER_SFML_HPP_

    #include "engine/render/IRenderer.hpp"
    #include "engine/log/Logger.hpp"
    #include <SFML/Graphics.hpp>
    #include <unordered_map>

namespace aer::render
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

            bool hasFocus(void) override;

            void resize(int width, int height) override;

            std::uint32_t loadTexture(const std::string& path) override;

            void unloadTexture(std::uint32_t id) override;

            std::uint32_t loadFont(const std::string& path) override;

            void unloadFont(std::uint32_t id) override;

            std::vector<aer::input::Event> pollEvents(void) override;

        private:
            /**
             * @brief Convert RGBA float components to an SFML Color
             * @param r Red component (0.0 to 1.0)
             * @param g Green component (0.0 to 1.0)
             * @param b Blue component (0.0 to 1.0)
             * @param a Alpha component (0.0 to 1.0)
             * @return Corresponding sf::Color object
             */
            auto toColor(float r, float g, float b, float a) -> sf::Color;

            /**
             * @brief Draw rectangle shapes from the render frame
             * @param frame The render frame containing shapes to draw
             */
            void drawShapes(const RenderFrame& frame);

            /**
             * @brief Draw sprites from the render frame
             * @param frame The render frame containing sprites to draw
             */
            void drawSprites(const RenderFrame& frame);

            /**
             * @brief Draw text elements from the render frame
             * @param frame The render frame containing texts to draw
             */
            void drawTexts(const RenderFrame& frame);

        private:
            std::string _name{"SFML"};              /**< Name of the renderer */

            sf::RenderWindow _window;               /**< SFML Render Window instance */

            std::unordered_map<std::uint32_t,       
                sf::Texture> _textures;             /**< Loaded textures mapped by their IDs */
            std::unordered_map<std::uint32_t,
                sf::Font> _fonts;                   /**< Loaded fonts mapped by their IDs */
            std::uint32_t _nextTextureId{1};        /**< Next available texture ID */
            std::uint32_t _nextFontId{1};           /**< Next available font ID */
    };
} // namespace aer::render

extern "C" {
    /**
     * @brief Factory function to create an instance of the SFML renderer
     * @return Pointer to the created IRenderer instance
     */
    aer::render::IRenderer* CreateRenderer();

    /**
     * @brief Function to destroy an instance of the SFML renderer
     * @param renderer Pointer to the IRenderer instance to be destroyed
     */
    void DestroyRenderer(aer::render::IRenderer* renderer);
}

#endif // ENGINE_RENDER_SFML_HPP_

