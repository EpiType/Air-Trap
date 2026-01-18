/**
 * File   : Network.cpp
 * License: MIT
 * Author : Elias Josué HAJJAR LLAUQUEN <elias-josue.hajjar-llauquen@epitech.eu>
 * Date   : 14/01/2026
 */

#include "engine/net/ANetwork.hpp"

namespace aer::net
{
    ////////////////////////////////////////////////////////////////////////////
    // Public Methods
    ////////////////////////////////////////////////////////////////////////////
    
    std::optional<NetworkEvent> ANetwork::pollEvent(void)
    {
        std::lock_guard lock{_eventMutex};
        if (_events.empty()) {
            return std::nullopt;
        }
        NetworkEvent event = std::move(_events.front());
        _events.pop_front();
        return event;
    }

    ////////////////////////////////////////////////////////////////////////////
    // Protected Methods
    ////////////////////////////////////////////////////////////////////////////

    void ANetwork::pushEvent(NetworkEvent event)
    {
        std::lock_guard lock{_eventMutex};
        _events.push_back(std::move(event));
    }
}