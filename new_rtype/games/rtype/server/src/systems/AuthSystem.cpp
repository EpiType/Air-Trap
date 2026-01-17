/**
 * File   : AuthSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 16/01/2026
 */

#include "rtype/systems/AuthSystem.hpp"

#include "engine/core/Logger.hpp"

#include <cstring>
#include <fstream>

namespace rtp::server::systems
{

    /////////////////////////////////////////////////////////////////////////
    // Public API
    /////////////////////////////////////////////////////////////////////////

    AuthSystem::AuthSystem(engine::net::INetwork &network)
        : _network(network)
    {
    }

    std::pair<bool, std::string> AuthSystem::handleLoginRequest(uint32_t sessionId,
                                                                const rtp::net::Packet &packet)
    {
        std::ifstream inFile("login.txt");

        rtp::net::LoginPayload payload{};
        rtp::net::Packet tmp = packet;
        tmp >> payload;

        std::string username(payload.username);
        std::string password(payload.password);

        engine::core::info("Login attempt: session={} username='{}' password='{}'",
                           sessionId, username, password);

        if (!inFile) {
            engine::core::error("Failed to open login.txt for reading");
            sendLoginResponse(sessionId, false, username);
            return {false, username};
        }

        const std::string record = username + ":" + password;
        std::string line;
        while (std::getline(inFile, line)) {
            if (line == record) {
                engine::core::info("Login successful for username '{}'", username);
                sendLoginResponse(sessionId, true, username);
                return {true, username};
            }
        }

        engine::core::warning("Login failed for username '{}'", username);
        sendLoginResponse(sessionId, false, username);
        return {false, username};
    }

    std::pair<bool, std::string> AuthSystem::handleRegisterRequest(uint32_t sessionId,
                                                                   const rtp::net::Packet &packet)
    {
        std::ofstream outFile("login.txt", std::ios::app);

        rtp::net::RegisterPayload payload{};
        rtp::net::Packet tmp = packet;
        tmp >> payload;

        std::string username(payload.username);
        std::string password(payload.password);

        engine::core::info("Registration attempt: username='{}' password='{}'",
                           username, password);

        if (!outFile) {
            engine::core::error("Failed to open login.txt for writing");
            sendRegisterResponse(sessionId, false, username);
            return {false, username};
        }

        if (username.find(':') != std::string::npos || password.find(':') != std::string::npos) {
            engine::core::warning("Registration failed: username or password contains ':'");
            sendRegisterResponse(sessionId, false, username);
            return {false, username};
        }

        std::ifstream inFile("login.txt");
        std::string line;
        while (std::getline(inFile, line)) {
            auto delimPos = line.find(':');
            if (delimPos != std::string::npos) {
                if (line.substr(0, delimPos) == username) {
                    engine::core::warning("Registration failed: username '{}' already exists", username);
                    sendRegisterResponse(sessionId, false, username);
                    return {false, username};
                }
            }
        }

        outFile << username << ':' << password << '\n';
        engine::core::info("Registration successful for username '{}'", username);
        sendRegisterResponse(sessionId, true, username);
        return {true, username};
    }

    /////////////////////////////////////////////////////////////////////////
    // Private API
    /////////////////////////////////////////////////////////////////////////

    void AuthSystem::sendLoginResponse(uint32_t sessionId,
                                       bool success,
                                       const std::string &username)
    {
        rtp::net::Packet response(rtp::net::OpCode::LoginResponse);
        rtp::net::LoginResponsePayload payload{};
        payload.success = success ? 1 : 0;
        std::strncpy(payload.username, username.c_str(), sizeof(payload.username) - 1);
        payload.username[sizeof(payload.username) - 1] = '\0';
        response << payload;

        const auto data = response.serialize();
        _network.sendPacket(sessionId, data, engine::net::NetChannel::TCP);
    }

    void AuthSystem::sendRegisterResponse(uint32_t sessionId,
                                          bool success,
                                          const std::string &username)
    {
        rtp::net::Packet response(rtp::net::OpCode::RegisterResponse);
        rtp::net::RegisterResponsePayload payload{};
        payload.success = success ? 1 : 0;
        std::strncpy(payload.username, username.c_str(), sizeof(payload.username) - 1);
        payload.username[sizeof(payload.username) - 1] = '\0';
        response << payload;

        const auto data = response.serialize();
        _network.sendPacket(sessionId, data, engine::net::NetChannel::TCP);
    }
}
