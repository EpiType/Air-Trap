/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Assert.hpp
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
 * @file Assert.hpp
 * @brief Assertion and verification macros for runtime checks
 * @author Robin Toillon
 * @details Provides platform-independent assertion macros with
 * different behaviors in debug and release builds.
 */

#ifndef RTYPE_ASSERT_HPP_
    #define RTYPE_ASSERT_HPP_

    #include "RType/Logger.hpp"
    #include <cstdlib>

    /**
     * @def RTP_DEBUG_BREAK
     * @brief Platform-specific debug break macro
     * @details Triggers a debugger breakpoint or aborts execution
     * depending on platform:
     * - MSVC: Uses __debugbreak()
     * - GCC/Clang: Uses __builtin_trap()
     * - Other: Uses std::abort()
     */
    #if defined(_MSC_VER)
        #define RTP_DEBUG_BREAK() __debugbreak()
    #elif defined(__GNUC__) || defined(__clang__)
        #define RTP_DEBUG_BREAK() __builtin_trap()
    #else
        #define RTP_DEBUG_BREAK() std::abort()
    #endif

    #ifdef NDEBUG
        /**
         * @def RTP_ASSERT
         * @brief Assertion macro (Release mode)
         * @details In release builds (NDEBUG defined), this macro is
         * optimized away:
         * - If C++23 assume attribute is available, hints to compiler
         *   the condition is true
         * - Otherwise, compiles to no-op
         * 
         * @param condition The condition to assert
         * @param ... Format string and optional arguments (unused in
         * release)
         */
        #ifdef __cpp_attribute_assume
            #define RTP_ASSERT(condition, ...) [[assume(condition)]]
        #else
            #define RTP_ASSERT(condition, ...) ((void)0)
        #endif

        /**
         * @def RTP_VERIFY
         * @brief Verification macro (Release mode)
         * @details In release builds, logs an error if condition fails
         * but continues execution.
         * 
         * @param condition The condition to verify
         * @param msg Error message format string
         * @param ... Optional format arguments
         */
        #define RTP_VERIFY(condition, msg, ...) \
            do { \
                if (!(condition)) [[unlikely]] { \
                    rtp::log::error("Verify failed: " msg, ##__VA_ARGS__); \
                } \
            } while(0)

    #else
        /**
         * @def RTP_ASSERT
         * @brief Assertion macro (Debug mode)
         * @details In debug builds (NDEBUG not defined), logs a fatal
         * error and breaks into debugger if condition is false.
         * 
         * @param condition The condition to assert
         * @param msg Error message format string
         * @param ... Optional format arguments
         * @note Program execution stops if assertion fails
         */
        #define RTP_ASSERT(condition, msg, ...) \
            do { \
                if (!(condition)) [[unlikely]] { \
                    rtp::log::fatal("ASSERTION FAILED: " msg, ##__VA_ARGS__); \
                    RTP_DEBUG_BREAK(); \
                } \
            } while(0)
        
        /**
         * @def RTP_VERIFY
         * @brief Verification macro (Debug mode)
         * @details In debug builds, behaves identically to RTP_ASSERT.
         * 
         * @param condition The condition to verify
         * @param msg Error message format string
         * @param ... Optional format arguments
         */
        #define RTP_VERIFY(condition, msg, ...) \
                RTP_ASSERT(condition, msg, ##__VA_ARGS__)

    #endif

#endif /* !RTYPE_ASSERT_HPP_ */
