/*
** EPITECH PROJECT, 2026
** Air-Trap
** File description:
** EventManager.cpp, EventManager implementation
*/

#include "RType/Event/EventManager.hpp"

namespace rtp
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    void EventManager::dispatch(void)
    {
        std::vector<QueuedEvent> currentQueue = std::move(this->_eventQueue);

        for (const auto &[id, eventPtr] : currentQueue) {
            if (id < this->_subscribers.size())
                for (const auto &cb : this->_subscribers[id])
                    cb(*eventPtr);
        }
    }

    void EventManager::clear(void)
    {
        this->_subscribers.clear();
        this->_eventQueue.clear();
    }
}
