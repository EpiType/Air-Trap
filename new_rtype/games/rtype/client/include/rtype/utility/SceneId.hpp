/**
 * File   : SceneId.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#ifndef RTYPE_CLIENT_SCENEID_HPP_
    #define RTYPE_CLIENT_SCENEID_HPP_

namespace rtp::client
{
    /**
     * @enum SceneId
     * @brief Identifiers for different client scenes.
     */
    enum class SceneId {
        NotInit,       /**< Uninitialized state */
        Menu,          /**< Main menu state */
        Login,         /**< Login screen state */
        Lobby,         /**< Lobby state */
        CreateRoom,    /**< Create room state */
        RoomWaiting,   /**< Room waiting state */
        Playing,       /**< In-game playing state */
        Settings,      /**< Settings menu state */
        KeyBindings,   /**< Key bindings configuration state */
        GamepadSettings, /**< Gamepad settings configuration state */
        Paused         /**< Game paused state */
    };
} // namespace rtp::client

#endif /* !RTYPE_CLIENT_SCENEID_HPP_ */