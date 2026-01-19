/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** Main
*/

#include "Core/Application.hpp"

std::string parseServerIp(int ac, char** av)
{
    std::string serverIp = "127.0.0.1"; // Default IP

    for (int i = 1; i < ac; ++i) {
        std::string arg = av[i];
        if (arg == "--host" && i + 1 < ac) {
            serverIp = av[i + 1];
            ++i;
        }
    }
    return serverIp;
}

uint16_t parseServerPort(int ac, char** av)
{
    uint16_t serverPort = 5000; // Default port

    for (int i = 1; i < ac; ++i) {
        std::string arg = av[i];
        if (arg == "--port" && i + 1 < ac) {
            serverPort = static_cast<uint16_t>(std::stoi(av[i + 1]));
            ++i;
        }
    }
    return serverPort;
}

int main(int ac, char** av)
{
    std::string serverIp = parseServerIp(ac, av);
    uint16_t serverPort = parseServerPort(ac, av);
    
    rtp::client::Application app(serverIp, serverPort);
    app.run();
    return 0;
}
