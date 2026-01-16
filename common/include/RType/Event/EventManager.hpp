/*
** EPITECH PROJECT, 2026
** Air-Trap
** File description:
** EventManager.hpp, EventManager definition
*/

#ifndef RTYPE_EVENT_EVENTMANAGER_HPP_
    #define RTYPE_EVENT_EVENTMANAGER_HPP_

    #include <concepts>
    #include <functional>
    #include <memory>
    #include <type_traits>
    #include <typeindex>
    #include <vector>
    #include "IEvent.hpp"

namespace rtp
{
    class EventManager {
        public:
            EventManager() = default;
            ~EventManager() = default;

            EventManager(const EventManager&) = delete;
            EventManager& operator=(const EventManager&) = delete;

            EventManager(EventManager&&) = default;
            EventManager& operator=(EventManager&&) = default;

            /**
             * @brief Subscribe to events of type T with the given callback.
             * @tparam T The event type to subscribe to.
             * @tparam F The type of the callback function.
             * @param callback The callback function to be invoked when an event of type T is published
             */
            template <typename T>
            void subscribe(std::invocable<const T&> auto &&callback)
                requires std::is_base_of_v<IEvent, T>;

            /**
             * @brief Queue an event of type T,
             * with the given arguments to be published later.
             * @tparam T The event type to queue.
             * @tparam Args The types of the arguments to construct the event.
             * @param args The arguments to construct the event.
             */
            template <typename T, typename... Args>
            void push(Args &&...args)
                requires std::is_base_of_v<IEvent, T>;

            /**
             * @brief Instantly publish an event of type T,
             * with the given arguments.
             * @tparam T The event type to publish.
             * @tparam Args The types of the arguments to construct the event.
             * @param args The arguments to construct the event.
             */
            template <typename T, typename... Args>
            void publish(Args &&...args)
                requires std::is_base_of_v<IEvent, T>;

            /**
             * @brief Dispatch all queued events to their subscribers.
             * @note After dispatching, the event queue is cleared.
             */
            void dispatch(void);

            /**
             * @brief Clear all subscribers and queued events.
             */
            void clear(void);

        private:
            struct QueuedEvent {
                std::size_t typeId;
                std::unique_ptr<IEvent> data;
            };
            std::size_t _nextEventID{0};
            std::unordered_map<std::type_index, std::size_t> _typeToId;
            std::vector<std::vector<std::function<void(const IEvent &)>>> _subscribers;
            std::vector<QueuedEvent> _eventQueue;


            template <typename T>
            std::size_t internalID(void);
    };
}

    #include "EventManager.tpp"

#endif /* !RTYPE_EVENT_EVENTMANAGER_HPP_ */
