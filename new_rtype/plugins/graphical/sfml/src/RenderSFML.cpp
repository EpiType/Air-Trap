/**
 * File   : RenderSFML.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#include "RenderSFML.hpp"
#include <unordered_map>

namespace engine::render
{
    namespace
    {
        const std::unordered_map<sf::Keyboard::Key, input::KeyCode> kKeyMap = {
            {sf::Keyboard::Key::Up, input::KeyCode::Up},
            {sf::Keyboard::Key::Down, input::KeyCode::Down},
            {sf::Keyboard::Key::Left, input::KeyCode::Left},
            {sf::Keyboard::Key::Right, input::KeyCode::Right},
            {sf::Keyboard::Key::Escape, input::KeyCode::Escape},
            {sf::Keyboard::Key::Enter, input::KeyCode::Return},
            {sf::Keyboard::Key::Space, input::KeyCode::Space},
            {sf::Keyboard::Key::F1, input::KeyCode::F1},
            {sf::Keyboard::Key::A, input::KeyCode::A},
            {sf::Keyboard::Key::B, input::KeyCode::B},
            {sf::Keyboard::Key::C, input::KeyCode::C},
            {sf::Keyboard::Key::D, input::KeyCode::D},
            {sf::Keyboard::Key::E, input::KeyCode::E},
            {sf::Keyboard::Key::F, input::KeyCode::F},
            {sf::Keyboard::Key::G, input::KeyCode::G},
            {sf::Keyboard::Key::H, input::KeyCode::H},
            {sf::Keyboard::Key::I, input::KeyCode::I},
            {sf::Keyboard::Key::J, input::KeyCode::J},
            {sf::Keyboard::Key::K, input::KeyCode::K},
            {sf::Keyboard::Key::L, input::KeyCode::L},
            {sf::Keyboard::Key::M, input::KeyCode::M},
            {sf::Keyboard::Key::N, input::KeyCode::N},
            {sf::Keyboard::Key::O, input::KeyCode::O},
            {sf::Keyboard::Key::P, input::KeyCode::P},
            {sf::Keyboard::Key::Q, input::KeyCode::Q},
            {sf::Keyboard::Key::R, input::KeyCode::R},
            {sf::Keyboard::Key::S, input::KeyCode::S},
            {sf::Keyboard::Key::T, input::KeyCode::T},
            {sf::Keyboard::Key::U, input::KeyCode::U},
            {sf::Keyboard::Key::V, input::KeyCode::V},
            {sf::Keyboard::Key::W, input::KeyCode::W},
            {sf::Keyboard::Key::X, input::KeyCode::X},
            {sf::Keyboard::Key::Y, input::KeyCode::Y},
            {sf::Keyboard::Key::Z, input::KeyCode::Z},
            {sf::Keyboard::Key::Num0, input::KeyCode::Num0},
            {sf::Keyboard::Key::Num1, input::KeyCode::Num1},
            {sf::Keyboard::Key::Num2, input::KeyCode::Num2},
            {sf::Keyboard::Key::Num3, input::KeyCode::Num3},
            {sf::Keyboard::Key::Num4, input::KeyCode::Num4},
            {sf::Keyboard::Key::Num5, input::KeyCode::Num5},
            {sf::Keyboard::Key::Num6, input::KeyCode::Num6},
            {sf::Keyboard::Key::Num7, input::KeyCode::Num7},
            {sf::Keyboard::Key::Num8, input::KeyCode::Num8},
            {sf::Keyboard::Key::Num9, input::KeyCode::Num9}
        };

        input::MouseButton toMouseButton(sf::Mouse::Button btn)
        {
            switch (btn) {
                case sf::Mouse::Button::Left:
                    return input::MouseButton::Left;
                case sf::Mouse::Button::Right:
                    return input::MouseButton::Right;
                case sf::Mouse::Button::Middle:
                    return input::MouseButton::Middle;
                default:
                    return input::MouseButton::Unknown;
            }
        }

        input::KeyCode toKeyCode(sf::Keyboard::Key key)
        {
            const auto it = kKeyMap.find(key);
            return (it == kKeyMap.end()) ? input::KeyCode::Unknown : it->second;
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    // Public API
    ////////////////////////////////////////////////////////////////////////////

    bool RenderSFML::init(int width, int height, const std::string& title)
    {
        engine::core::info("Initializing SFML Renderer: {}x{} - '{}'", width, height, title);
        _window.create(
            sf::VideoMode({static_cast<unsigned int>(width),
                           static_cast<unsigned int>(height)}),
            title);
        if (!_window.isOpen()) {
            return false;
        }
        _window.setVerticalSyncEnabled(true);
        return true;
    }

    void RenderSFML::shutdown(void)
    {
        if (_window.isOpen()) {
            _window.close();
        }
    }

    std::string RenderSFML::getName(void) const
    {
        return _name;
    }

    void RenderSFML::beginFrame(void)
    {
        _window.clear(sf::Color::Black);
    }

    void RenderSFML::draw(const RenderFrame& frame)
    {
        drawShapes(frame);
        drawSprites(frame);
        drawTexts(frame);
    }

    void RenderSFML::endFrame(void)
    {
        _window.display();
    }

    bool RenderSFML::hasFocus(void)
    {
        return _window.hasFocus();
    }

    void RenderSFML::resize(int width, int height)
    {
        _window.setSize({static_cast<unsigned int>(width), static_cast<unsigned int>(height)});
    }

    std::uint32_t RenderSFML::loadTexture(const std::string& path)
    {
        sf::Texture texture;
        if (!texture.loadFromFile(path)) {
            return 0;
        }
        std::uint32_t id = _nextTextureId++;
        _textures[id] = std::move(texture);
        return id;
    }

    void RenderSFML::unloadTexture(std::uint32_t id)
    {
        _textures.erase(id);
    }

    std::uint32_t RenderSFML::loadFont(const std::string& path)
    {
        sf::Font font;
        if (!font.openFromFile(path)) {
            engine::core::warning("RenderSFML: failed to load font '{}'", path);
            return 0;
        }
        std::uint32_t id = _nextFontId++;
        _fonts[id] = std::move(font);
        return id;
    }

    void RenderSFML::unloadFont(std::uint32_t id)
    {
        _fonts.erase(id);
    }

    std::vector<engine::input::Event> RenderSFML::pollEvents(void)
    {
        std::vector<input::Event> outEvents;
        if (!_window.isOpen()) {
            return outEvents;
        }

        while (const std::optional event = _window.pollEvent())
        {
            if (event->is<sf::Event::Closed>()) {
                input::Event e{};
                e.type = input::EventType::Close;
                outEvents.push_back(e);
                continue;
            }

            if (const auto *kp = event->getIf<sf::Event::KeyPressed>()) {
                input::Event e{};
                e.type = input::EventType::KeyDown;
                e.key = toKeyCode(kp->code);
                outEvents.push_back(e);
                continue;
            }

            if (const auto *kr = event->getIf<sf::Event::KeyReleased>()) {
                input::Event e{};
                e.type = input::EventType::KeyUp;
                e.key = toKeyCode(kr->code);
                outEvents.push_back(e);
                continue;
            }

            if (const auto *te = event->getIf<sf::Event::TextEntered>()) {
                input::Event e{};
                e.type = input::EventType::TextEntered;
                e.text = static_cast<char32_t>(te->unicode);
                outEvents.push_back(e);
                continue;
            }

            if (const auto *mm = event->getIf<sf::Event::MouseMoved>()) {
                input::Event e{};
                e.type = input::EventType::MouseMove;
                e.x = mm->position.x;
                e.y = mm->position.y;
                outEvents.push_back(e);
                continue;
            }
            
            if (const auto *mp = event->getIf<sf::Event::MouseButtonPressed>()) {
                input::Event e{};
                e.type = input::EventType::MouseButtonDown;
                e.mouseButton = toMouseButton(mp->button);
                e.x = mp->position.x;
                e.y = mp->position.y;
                outEvents.push_back(e);
                continue;
            }

            if (const auto *mr = event->getIf<sf::Event::MouseButtonReleased>()) {
                input::Event e{};
                e.type = input::EventType::MouseButtonUp;
                e.mouseButton = toMouseButton(mr->button);
                e.x = mr->position.x;
                e.y = mr->position.y;
                outEvents.push_back(e);
                continue;
            }

            if (const auto *mw = event->getIf<sf::Event::MouseWheelScrolled>()) {
                input::Event e{};
                e.type = input::EventType::MouseWheel;
                e.wheelDelta = mw->delta;
                e.x = mw->position.x;
                e.y = mw->position.y;
                outEvents.push_back(e);
                continue;
            }
        }
        return outEvents;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Private API
    ///////////////////////////////////////////////////////////////////////////

    auto RenderSFML::toColor(float r, float g, float b, float a) -> sf::Color
    {
        return sf::Color(
            static_cast<std::uint8_t>(r * 255.0f),
            static_cast<std::uint8_t>(g * 255.0f),
            static_cast<std::uint8_t>(b * 255.0f),
            static_cast<std::uint8_t>(a * 255.0f));
    }

    void RenderSFML::drawShapes(const RenderFrame& frame)
    {
        auto shapes = frame.shapes;
        std::sort(shapes.begin(), shapes.end(),
                  [](const RenderRect &a, const RenderRect &b) { return a.z < b.z; });
        for (const auto &rect : shapes) {
            sf::RectangleShape shape;
            shape.setSize({rect.size.x, rect.size.y});
            shape.setPosition({rect.position.x, rect.position.y});
            if (rect.filled) {
                shape.setFillColor(toColor(rect.r, rect.g, rect.b, rect.a));
            } else {
                shape.setFillColor(sf::Color::Transparent);
                shape.setOutlineThickness(rect.thickness);
                shape.setOutlineColor(toColor(rect.r, rect.g, rect.b, rect.a));
            }
            _window.draw(shape);
        }
    }

    void RenderSFML::drawSprites(const RenderFrame& frame)
    {
        auto sprites = frame.sprites;
        std::sort(sprites.begin(), sprites.end(),
                  [](const RenderSprite &a, const RenderSprite &b) { return a.z < b.z; });
        for (const auto& sprite : sprites) {
            if (sprite.textureId != 0 && _textures.find(sprite.textureId) != _textures.end()) {
                sf::Sprite s(_textures[sprite.textureId]);
                if (sprite.srcSize.x > 0.0f && sprite.srcSize.y > 0.0f) {
                    s.setTextureRect(sf::IntRect(
                        {static_cast<int>(sprite.srcOffset.x),
                         static_cast<int>(sprite.srcOffset.y)},
                        {static_cast<int>(sprite.srcSize.x),
                         static_cast<int>(sprite.srcSize.y)}));
                }
                s.setPosition({sprite.position.x, sprite.position.y});
                s.setOrigin({sprite.origin.x, sprite.origin.y});
                s.setRotation(sf::degrees(sprite.rotation));
                s.setScale({sprite.scale.x, sprite.scale.y});
                s.setColor(toColor(sprite.r, sprite.g, sprite.b, sprite.a));
                _window.draw(s);
            }
        }
    }

    void RenderSFML::drawTexts(const RenderFrame& frame)
    {
        auto texts = frame.texts;
        std::sort(texts.begin(), texts.end(),
                  [](const RenderText &a, const RenderText &b) { return a.z < b.z; });
        for (const auto &text : texts) {
            if (text.fontId == 0 || _fonts.find(text.fontId) == _fonts.end()) {
                continue;
            }
            sf::Text t(_fonts[text.fontId]);
            t.setString(text.content);
            t.setCharacterSize(static_cast<unsigned int>(text.size));
            t.setPosition({text.position.x, text.position.y});
            t.setScale({text.scale.x, text.scale.y});
            t.setFillColor(toColor(text.r, text.g, text.b, text.a));

            _window.draw(t);
        }
    }

} // namespace engine::render

extern "C"
{
    engine::render::IRenderer* CreateRenderer()
    {
        return new engine::render::RenderSFML();
    }

    void DestroyRenderer(engine::render::IRenderer* renderer)
    {
        delete renderer;
    }
}
