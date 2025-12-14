/**
 * File   : main.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 10/12/2025
 */

#include "Network/Network.hpp"

using namespace rtp::server::net;

int main(void) {
    Network server(12345);
    server.start();

    // simulation
    std::this_thread::sleep_for(std::chrono::hours(24));

    return 0;
}