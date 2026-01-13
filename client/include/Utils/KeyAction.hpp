/**
 * File   : KeyAction.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_CLIENT_UTILS_KEYACTION_HPP_
    #define RTYPE_CLIENT_UTILS_KEYACTION_HPP_

namespace rtp::client {
    /**
     * @enum KeyAction
     * @brief Actions that can be bound to keys
     */
    enum class KeyAction {
        MoveUp,             /**< Action for moving up */
        MoveDown,           /**< Action for moving down */
        MoveLeft,           /**< Action for moving left */
        MoveRight,          /**< Action for moving right */
        Shoot,              /**< Action for shooting */
        Reload,             /**< Action for reloading */
        Pause,              /**< Action for pausing the game */
        Menu                /**< Action for opening the menu */
    };
} // namespace rtp::client

#endif // RTYPE_CLIENT_UTILS_KEYACTION_HPP_
