/**
 * File   : KeyAction.hpp
 * License: MIT
 * Author : Elias Josue HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 */

#ifndef RTYPE_CLIENT_KEYACTION_HPP_
    #define RTYPE_CLIENT_KEYACTION_HPP_

namespace rtp::client
{
    /**
     * @enum KeyAction
     * @brief Logical input actions used for key bindings.
     */
    enum class KeyAction {
        MoveUp,
        MoveDown,
        MoveLeft,
        MoveRight,
        Shoot,
        Reload,
        Pause
    };
}

#endif /* !RTYPE_CLIENT_KEYACTION_HPP_ */
