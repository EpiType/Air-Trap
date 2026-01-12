/**
 * File   : GameState.hpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#ifndef RTYPE_CLIENT_UTILS_GAMESTATE_HPP_
    #define RTYPE_CLIENT_UTILS_GAMESTATE_HPP_

namespace rtp::client {

    /**
     * @class Application
     * @brief Main application class for the R-Type client.
     *
     * This class initializes and runs the main application loop,
     * handling window creation, ECS setup, and system management.
     */
    enum class GameState {
        Menu,          /**< Main menu state */
        Login,         /**< Login screen state */
        Lobby,         /**< Lobby state */
        CreateRoom,    /**< Create room state */
        RoomWaiting,   /**< Room waiting state */
        Playing,       /**< In-game playing state */
        Settings,      /**< Settings menu state */
        KeyBindings,   /**< Key bindings configuration state */
        Paused         /**< Game paused state */
    };
} // namespace rtp::client

#endif // RTYPE_CLIENT_UTILS_GAMESTATE_HPP_