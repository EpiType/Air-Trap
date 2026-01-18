/**
 * File   : AuthSystem.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 11/12/2025
 */

#include "Systems/AuthSystem.hpp"
#include <fstream>

namespace rtp::server {

    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    AuthSystem::AuthSystem(ServerNetwork& network, ecs::Registry& registry)
        : _network(network), _registry(registry) {}; 

    void AuthSystem::update(float dt) 
    {
        (void)dt;
    };

    std::tuple<bool, std::string, uint8_t> AuthSystem::handleLoginRequest(uint32_t sessionId, const net::Packet& packet)
    {
        std::ifstream inFile("logins.txt");

        std::string username;
        std::string password;

        net::LoginPayload payload;
        net::Packet tempPacket = packet;
        tempPacket >> payload;
        username = std::string(payload.username);
        password = std::string(payload.password);
        uint8_t weaponKind = payload.weaponKind;

        log::info("Login attempt: session={} username='{}' password='{}'",
                       sessionId, username, password);

        if (!inFile) {
            log::error("Failed to open logins.txt for reading");
            sendLoginResponse(sessionId, false, username);
            return {false, username, weaponKind};
        }

        std::string line;
        std::string record = username + ":" + password;

        while (std::getline(inFile, line)) {
            if (line == record) {
                log::info("Login successful for username '{}'", username);
                sendLoginResponse(sessionId, true, username);
                return {true, username, weaponKind};
            }
        }
        log::warning("Login failed for username '{}'", username);
        sendLoginResponse(sessionId, false, username);
        return {false, username, weaponKind};
    }

    std::pair<bool, std::string> AuthSystem::handleRegisterRequest(uint32_t sessionId, const net::Packet& packet)
    {
        std::ofstream outFile("logins.txt", std::ios::app);

        std::string username;
        std::string password;

        net::RegisterPayload payload;
        net::Packet tempPacket = packet;
        tempPacket >> payload;
        username = std::string(payload.username);
        password = std::string(payload.password);

        log::info("Registration attempt: username='{}' password='{}'",
                       username, password);

        if (!outFile) {
            log::error("Failed to open logins.txt for writing");
            sendRegisterResponse(sessionId, false, username);
            return {false, username};
        }

        if (username.find(':') != std::string::npos || password.find(':') != std::string::npos) {
            log::warning("Registration failed: username or password contains invalid character ':'");
            sendRegisterResponse(sessionId, false, username);
            return {false, username};
        }

        std::ifstream inFile("logins.txt");
        std::string line;
        while (std::getline(inFile, line)) {
            size_t delimPos = line.find(':');
            if (delimPos != std::string::npos) {
                std::string existingUsername = line.substr(0, delimPos);
                if (existingUsername == username) {
                    log::warning("Registration failed: username '{}' already exists", username);
                    sendRegisterResponse(sessionId, false, username);
                    return {false, username};
                }
            }
        }
        
        outFile << username << ":" << password << "\n";
        log::info("Registration successful for username '{}'", username);
        sendRegisterResponse(sessionId, true, username);
        return {true, username};
    }

    void AuthSystem::sendLoginResponse(uint32_t sessionId, bool success, const std::string& username)
    {
        net::Packet responsePacket(net::OpCode::LoginResponse);
        net::LoginResponsePayload payload{};
        payload.success = success;
        std::strncpy(payload.username, username.c_str(), sizeof(payload.username) - 1);
        payload.username[sizeof(payload.username) - 1] = '\0';
        responsePacket << payload;
        _network.sendPacket(sessionId, responsePacket, net::NetworkMode::TCP);
    }

    void AuthSystem::sendRegisterResponse(uint32_t sessionId, bool success, const std::string& username)
    {
        net::Packet responsePacket(net::OpCode::RegisterResponse);
        net::RegisterResponsePayload payload{};
        payload.success = success;
        std::strncpy(payload.username, username.c_str(), sizeof(payload.username) - 1);
        payload.username[sizeof(payload.username) - 1] = '\0';
        responsePacket << payload;
        _network.sendPacket(sessionId, responsePacket, net::NetworkMode::TCP);
    }

} // namespace rtp::server