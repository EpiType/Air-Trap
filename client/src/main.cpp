#include <iostream>
#include <thread>
#include <chrono>

/**
 * @brief Client entry point.
 * 
 * Client workflow:
 * 1. Capture player input
 * 2. Send inputs to server
 * 3. Receive game state from server
 * 4. Render the game
 */

int main(int argc, char* argv[]) {
    std::cout << "R-Type Client starting..." << std::endl;

    // Parse command-line arguments (server IP, port)
    std::string serverIp = "127.0.0.1";
    std::uint16_t serverPort = 4242;
    
    if (argc > 1) {
        serverIp = argv[1];
    }
    if (argc > 2) {
        serverPort = static_cast<std::uint16_t>(std::atoi(argv[2]));
    }

    std::cout << "Would connect to server " << serverIp << ":" << serverPort << std::endl;
    std::cout << "Architecture initialized. Implement game logic here." << std::endl;

    // Game loop placeholder
    std::cout << "Press Ctrl+C to stop..." << std::endl;
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }

    return 0;
}
