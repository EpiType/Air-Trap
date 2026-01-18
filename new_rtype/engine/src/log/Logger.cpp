/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Logger.cpp, Logger class implementation
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
 * @file Logger.cpp
 * @brief Implementation of the logging system
 * @author Robin Toillon
 * @details Implements thread-safe logging with colored console output,
 * file logging support, automatic source location tracking, and
 * timestamp formatting.
 */

#include "engine/log/Logger.hpp"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <print>

namespace aer::log
{
    namespace detail
    {
        class LoggerBackend final {
            public:
                LoggerBackend() noexcept = default;

                ~LoggerBackend() noexcept
                {
                    if (this->_file.is_open()) {
                        this->_file.close();
                    }
                }

                void setOutputFile(std::string_view filename)
                {
                    std::lock_guard lock(this->_mutex);
                    if (this->_file.is_open()) {
                        this->_file.close();
                    }
                    this->_file.open(std::string(filename), std::ios::app);
                }

                void write(Level level, std::string_view msg,
                           const std::source_location &loc)
                {
                    auto now = std::chrono::system_clock::now();
                    auto timePoint = std::chrono::floor<std::chrono::seconds>(
                        now);
                    auto cleanFileName = std::filesystem::path(
                        loc.file_name()).filename().string();

                    std::string_view color = "";

                    switch (level) {
                        using enum Level;
                        case Debug:
                            color = "\033[36m";
                            break;
                        case Info:
                            color = "\033[32m";
                            break;
                        case Warning:
                            color = "\033[33m";
                            break;
                        case Error:
                            color = "\033[31m";
                            break;
                        case Fatal:
                            color = "\033[1;31m";
                            break;
                        case None:
                            color = "\033[90m";
                            break;
                    }

                    std::lock_guard lock{this->_mutex};

                    std::println(std::cout, "{}[{:%T}] [{}] {} \033[0m "
                                             "\t({}:{})",
                                             color, timePoint, level, msg,
                                             loc.file_name(), loc.line());
                    if (this->_file.is_open()) {
                        std::println(this->_file, "[{:%T}] [{}] {} ({}:{})",
                                     timePoint, level, msg,
                                     cleanFileName, loc.line());
                    }
                }

            private:
                std::mutex _mutex;
                std::ofstream _file;
        };

        static LoggerBackend &getBackend(void) noexcept
        {
            static LoggerBackend instance;
            return instance;
        }

        void logImpl(Level level, std::string_view message,
                     const std::source_location &loc)
        {
            try {
                getBackend().write(level, message, loc);
            } catch (...) {
                std::println(std::cerr, "\033[31m[LOGGER FAILURE] {} \033[0m",
                             message);
            }
        }
    }

    auto configure(std::string_view logFilePath) noexcept
        -> std::expected<void, log::Error>
    {
        try {
            detail::getBackend().setOutputFile(logFilePath);
        } catch (const std::exception &e) {
            return std::unexpected{
                Error::failure(ErrorCode::InvalidParameter,
                               "Logger configuration failed: {}", e.what())};
        } catch (...) {
            return std::unexpected{
                Error::failure(ErrorCode::Unknown,
                               "Logger configuration failed: unknown error")};
        }

        return {};
    }
}
