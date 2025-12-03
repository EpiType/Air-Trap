/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** Logger.cpp, Logger class implementation
*/

#include "Core/Logger/Logger.hpp"

#include <chrono>
#include <filesystem>
#include <format>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>

namespace rtp::log
{
    namespace detail
    {
        class LoggerBackend {
            public:
                explicit LoggerBackend(void) noexcept
                {
                }

                ~LoggerBackend() noexcept {
                    if (this->_file.is_open()) {
                        this->_file.close();
                    }
                }

                void setOutputFile(std::string_view filename)
                {
                    std::lock_guard lock(this->_mutex);
                    if (this->_file.is_open())
                        this->_file.close();
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
                    std::string_view levelStr = "";

                    switch (level) {
                        using enum Level;
                        case Debug:
                            color = "\033[36m";
                            levelStr = "DEBUG";
                            break;
                        case Info:
                            color = "\033[32m";
                            levelStr = "INFO ";
                            break;
                        case Warning:
                            color = "\033[33m";
                            levelStr = "WARN ";
                            break;
                        case Error:
                            color = "\033[31m";
                            levelStr = "ERROR";
                            break;
                    }

                    std::lock_guard lock{this->_mutex};

                    std::cout << std::format("{}[{:%T}] [{}] {} \033[0m "
                                             "\t({}:{})\n",
                                             color, timePoint, levelStr, msg,
                                             loc.file_name(), loc.line());
                    if (this->_file.is_open()) {
                        this->_file << std::format("[{:%T}] [{}] {} ({}:{})\n", 
                                                   timePoint, levelStr, msg,
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
                std::cerr << "[LOGGER FAILURE] " << message << std::endl;
            }
        }
    }

    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    auto configure(std::string_view logFilePath) noexcept
        -> std::expected<void, std::string>
    {
        try {
            detail::getBackend().setOutputFile(logFilePath);
        } catch (const std::exception &e) {
            return std::unexpected{
                std::format("Logger configuration failed: {}", e.what())};
        } catch (...) {
            return std::unexpected{"Logger configuration failed: unknown error"};
        }

        return {};
    }
}
