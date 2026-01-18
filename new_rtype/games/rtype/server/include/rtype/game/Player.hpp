/**
 * File   : Player.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#ifndef RTYPE_SERVER_PLAYER_HPP_
    #define RTYPE_SERVER_PLAYER_HPP_

    #include "engine/math/Vec2.hpp"

    #include <cstdint>
    #include <string>

namespace rtp::server
{
    class Player {
        public:
            /** 
             * @brief Constructor for Player
             * @param sessionId Unique session identifier for the player
             */
            explicit Player(uint32_t sessionId);

            /** 
             * @brief Get the player's session ID.
             * @return Session ID
             */
            [[nodiscard]]
            uint32_t id(void) const;

            /** 
             * @brief Get the player's username.
             * @return Username string
             */
            [[nodiscard]]
            const std::string &username(void) const;

            /** 
             * @brief Set the player's username.
             * @param username New username string
             */
            void setUsername(std::string username);

            /** 
             * @brief Check if the player is logged in.
             * @return True if logged in, false otherwise
             */
            [[nodiscard]]
            bool loggedIn(void) const;

            /** 
             * @brief Set the player's logged-in status.
             * @param loggedIn New logged-in status
             */
            void setLoggedIn(bool loggedIn);

            /** 
             * @brief Check if the player is ready.
             * @return True if ready, false otherwise
             */
            [[nodiscard]]
            bool ready(void) const;

            /** 
             * @brief Set the player's ready status.
             * @param ready New ready status
             */
            void setReady(bool ready);

            /** 
             * @brief Get the room ID the player is in.
             * @return Room ID
             */
            [[nodiscard]]
            uint32_t roomId(void) const;

            /** 
             * @brief Set the room ID the player is in.
             * @param roomId New room ID
             */
            void setRoomId(uint32_t roomId);

            /** 
             * @brief Get the player's entity ID.
             * @return Entity ID
             */
            [[nodiscard]]
            uint32_t entityId(void) const;

            /** 
             * @brief Set the player's entity ID.
             * @param entityId New entity ID
             */
            void setEntityId(uint32_t entityId);

            /**
             * @brief Get the player's position.
             * @return Position vector
             */
            [[nodiscard]]
            aer::math::Vec2f position(void) const;

            /**
             * @brief Set the player's position.
             * @param position New position vector
             */
            void setPosition(const aer::math::Vec2f &position);

            /**
             * @brief Get the player's velocity.
             * @return Velocity vector
             */
            [[nodiscard]]
            aer::math::Vec2f velocity(void) const;

            /**<
             * @brief Set the player's velocity.
             * @param velocity New velocity vector
             */
            void setVelocity(const aer::math::Vec2f &velocity);

            /**
             * @brief Get the input mask representing player inputs.
             * @return Input mask as an 8-bit unsigned integer
             */
            [[nodiscard]]
            uint8_t inputMask(void) const;

            /**
             * @brief Set the input mask representing player inputs.
             * @param mask Input mask as an 8-bit unsigned integer
             */
            void setInputMask(uint8_t mask);

        private:
            uint32_t _sessionId{0};             /**< Unique session identifier for the player */
            std::string _username{"guest"};     /**< Username of the player */
            bool _loggedIn{false};              /**< Flag indicating if the player is logged in */
            bool _ready{false};                 /**< Flag indicating if the player is ready */
            uint32_t _roomId{0};                /**< Identifier of the room the player is in */
            uint32_t _entityId{0};              /**< Identifier of the player's entity in the game */
            aer::math::Vec2f
                _position{0.f, 0.f};            /**< Player's position in the game world */
            aer::math::Vec2f
                _velocity{0.f, 0.f};            /**< Player's velocity in the game world */
            uint8_t _inputMask{0};              /**< Input mask representing player inputs */
    };
}

#endif /* !RTYPE_SERVER_PLAYER_HPP_ */
