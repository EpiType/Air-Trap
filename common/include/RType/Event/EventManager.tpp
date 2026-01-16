/*
** EPITECH PROJECT, 2026
** Air-Trap
** File description:
** EventManager.tpp, EventManager templated implementation
*/

namespace rtp
{
    //////////////////////////////////////////////////////////////////////////
    // Public API
    //////////////////////////////////////////////////////////////////////////

    template <typename T>
    void EventManager::subscribe(std::invocable<const T &> auto &&callback)
        requires std::is_base_of_v<IEvent, T>
    {
        std::size_t id = this->internalID<T>();

        if (id >= _subscribers.size())
            this->_subscribers.resize(id + 1);

        this->_subscribers[id].push_back(
            [cb = std::forward<decltype(callback)>(callback)](
                const IEvent &event) { cb(static_cast<const T &>(event)); });
    }

    template <typename T, typename... Args>
    void EventManager::push(Args &&...args)
        requires std::is_base_of_v<IEvent, T>
    {
        this->_eventQueue.push_back(
            {this->internalID<T>(),
             std::make_unique<T>(std::forward<Args>(args)...)});
    }

    template <typename T, typename... Args>
    void EventManager::publish(Args &&...args)
        requires std::is_base_of_v<IEvent, T>
    {
        if (std::size_t id = this->internalID<T>();
            id < this->_subscribers.size())
            for (T event(std::forward<Args>(args)...);
                 auto const &cb : this->_subscribers[id])
                cb(event);
    }

    //////////////////////////////////////////////////////////////////////////
    // Private API
    //////////////////////////////////////////////////////////////////////////

    template <typename T>
    std::size_t EventManager::internalID(void)
    {
        const std::type_index typeIndex = std::type_index(typeid(T));
        if (auto it = this->_typeToId.find(typeIndex); it != this->_typeToId.end())
            return it->second;
        std::size_t newId = this->_nextEventID++;
        this->_typeToId[typeIndex] = newId;
        return newId;
    }
}
