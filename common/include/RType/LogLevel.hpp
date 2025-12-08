/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** LogLevel.hpp, LogLevel enumeration declaration
*/

/*
** MIT License
**
** Copyright (c) 2025 Robin Toillon
**
** Permission is hereby granted, free of charge, to any person obtaining
** a copy of this software and associated documentation files (the
** "Software"), to deal in the Software without restriction, including
** without limitation the rights to use, copy, modify, merge, publish,
** distribute, sublicense, and/or sell copies of the Software, and to
** permit persons to whom the Software is furnished to do so, subject to
** the following conditions:
**
** The above copyright notice and this permission notice shall be
** included in all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
** EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
** MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
** IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
** CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
** TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
** SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/**
 * @file LogLevel.hpp
 * @brief Log severity levels for the logging system
 * @author Robin Toillon
 * @details Defines the severity levels used throughout the logging
 * system.
 */

#ifndef RTYPE_LOGLEVEL_HPP_
    #define RTYPE_LOGLEVEL_HPP_

    #include <cstdint>

namespace rtp::log
{
    /**
     * @enum Level
     * @brief Severity levels for log messages
     * @details Ordered from least to most severe. Each level represents
     * a different category of log message importance.
     */
    enum class Level : std::uint8_t {
        Debug = 0,  /**< Detailed information for debugging purposes */
        Info,       /**< General informational messages */
        Warning,    /**< Warning messages for potentially harmful situations */
        Error,      /**< Error messages for error events */
        Fatal,      /**< Fatal error messages indicating critical failures */
        None        /**< No logging (used to disable logging) */
    };

}

#endif /* !RTYPE_LOGLEVEL_HPP_ */