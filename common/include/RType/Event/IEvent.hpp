/*
** EPITECH PROJECT, 2025
** Air-Trap
** File description:
** IEvent.hpp, IEvent interface 
*/

#ifndef RTYPE_IEVENT_HPP_
    #define RTYPE_IEVENT_HPP_

    #include <cstddef>

namespace rtp
{
    class IEvent {
        public:
            virtual ~IEvent() = default;

            virtual std::size_t getID(void) const noexcept = 0;
    };
}

#endif /* !RTYPE_IEVENT_HPP_ */
