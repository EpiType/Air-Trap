/**
 * File   : Input.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#ifndef ENGINE_INPUT_INPUT_HPP_
    #define ENGINE_INPUT_INPUT_HPP_

    #include "engine/input/Event.hpp"
    #include "engine/core/Logger.hpp"

    #include <array>
    #include <vector>

namespace engine::input
{
    class Input final {
        public:
            /**
             * @brief Get the singleton instance of the Input manager
             * @return Reference to the Input instance
             */
            static Input &instance(void);

            /*****************************/
            /* Frame Management
            /*****************************/

            /**
             * @brief Prepare for a new frame by resetting input states
             */
            void beginFrame(void);

            /**
             * @brief Process a list of input events for the current frame
             * @param events Vector of input events to process
             */
            void processEvents(const std::vector<Event> &events);

            /*****************************/
            /* Handle Key Inputs
            /*****************************/

            /**
             * @brief Check if a key is currently held down
             * @param key The key code to check
             * @return true if the key is down, false otherwise
             */
            [[nodiscard]]
            bool isKeyDown(KeyCode key) const;

            /**
             * @brief Check if a key was pressed this frame
             * @param key The key code to check
             * @return true if the key was pressed, false otherwise
             */
            [[nodiscard]]
            bool isKeyPressed(KeyCode key) const;

            /**
             * @brief Check if a key was released this frame
             * @param key The key code to check
             * @return true if the key was released, false otherwise
             */
            [[nodiscard]]
            bool isKeyReleased(KeyCode key) const;

            /*****************************/
            /* Handle Mouse Inputs
            /*****************************/

            /**
             * @brief Check if a mouse button is currently held down
             * @param button The mouse button to check
             * @return true if the button is down, false otherwise
             */
            [[nodiscard]]
            bool isMouseDown(MouseButton button) const;

            /**
             * @brief Check if a mouse button was pressed this frame
             * @param button The mouse button to check
             * @return true if the button was pressed, false otherwise
             */
            [[nodiscard]]
            bool isMousePressed(MouseButton button) const;

            /**
             * @brief Check if a mouse button was released this frame
             * @param button The mouse button to check
             * @return true if the button was released, false otherwise
             */
            [[nodiscard]]
            bool isMouseReleased(MouseButton button) const;

            /*****************************/
            /* Handle Mouse Position
            /*****************************/

            /**
             * @brief Get the current X position of the mouse
             * @return The X coordinate of the mouse
             */
            [[nodiscard]]
            int mouseX(void) const;

            /**
             * @brief Get the current Y position of the mouse
             * @return The Y coordinate of the mouse
             */
            [[nodiscard]]
            int mouseY(void) const;

            /**
             * @brief Get the accumulated mouse wheel delta for the current frame
             * @return The mouse wheel delta
             */
            [[nodiscard]]
            float wheelDelta(void) const;

            /*****************************/
            /* Handle Window Events
            /*****************************/

            /**
             * @brief Check if a close event was requested
             * @return true if a close was requested, false otherwise
             */
            [[nodiscard]] bool closeRequested(void) const;

            /*****************************/
            /* Handle Key Inputs
            /*****************************/

            /**
             * @brief Get the list of processed input events for the current frame
             * @return Const reference to the vector of input events
             */
            [[nodiscard]] const std::vector<Event> &events(void) const;

        private:
            /*****************************/
            /* Utility Functions
            /*****************************/

            /**
             * @brief Get the index of a key code for internal storage
             * @param key The key code
             * @return The index corresponding to the key code
             */
            [[nodiscard]]
            static std::size_t keyIndex(KeyCode key);
            
            /**
             * @brief Get the index of a mouse button for internal storage
             * @param button The mouse button
             * @return The index corresponding to the mouse button
             */
            [[nodiscard]]
            static std::size_t mouseIndex(MouseButton button);

        private:
            Input() = default;                      /**< Private constructor for singleton pattern */

            static constexpr std::size_t kKeyCount =
                static_cast<std::size_t>(KeyCode::Num9) + 1;                
            static constexpr std::size_t kMouseCount =
                static_cast<std::size_t>(MouseButton::Unknown) + 1;

            std::array<bool, kKeyCount>
                _keysDown{};                        /**< Key down states */
            std::array<bool, kKeyCount>
                _keysPressed{};                     /**< Key pressed states */
            std::array<bool, kKeyCount>
                _keysReleased{};                    /**< Key released states */

            std::array<bool, kMouseCount>
                _mouseDown{};                       /**< Mouse button down states */
            std::array<bool, kMouseCount>
                _mousePressed{};                    /**< Mouse button pressed states */
            std::array<bool, kMouseCount>
                _mouseReleased{};                   /**< Mouse button released states */

            int _mouseX{0};                         /**< Current mouse X position */
            int _mouseY{0};                         /**< Current mouse Y position */
            float _wheelDelta{0.0f};                /**< Accumulated mouse wheel delta */
            bool _closeRequested{false};            /**< Flag indicating if a close was requested */

            std::vector<Event> _events;             /**< List of processed input events for the current frame */
    };
}

#endif /* !ENGINE_INPUT_INPUT_HPP_ */
