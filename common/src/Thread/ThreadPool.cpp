/*
** EPITECH PROJECT, 2025
** The Plazza
** File description:
** ThreadPool.cpp, ThreadPool class implementation
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
 * @file ThreadPool.cpp
 * @brief ThreadPool class implementation
 * @author Robin Toillon
 * @details This file contains the implementation of the ThreadPool
 * class. The ThreadPool class manages a pool of worker threads that
 * can execute tasks asynchronously. Tasks can be enqueued to the pool,
 * and the worker threads will process them concurrently. The class
 * ensures thread-safe access to the task queue using mutexes and
 * condition variables.
 */

#include "RType/Assert.hpp"
#include "RType/Logger.hpp"
#include "RType/Thread/ThreadPool.hpp"

#include <format>
#include <exception>

namespace rtp::thread
{

    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    auto ThreadPool::create(size_t numThreads)
        -> std::expected<std::unique_ptr<ThreadPool>, rtp::Error>
    {
        constexpr std::string_view fmt{"ThreadPool init failed: {}"};

        if (numThreads == 0)
            return std::unexpected{
                Error::failure(ErrorCode::InvalidParameter,
                               fmt, "number of threads must be at least 1")};

        try {
            std::unique_ptr<ThreadPool> pool{new ThreadPool{}};

            pool->start(numThreads);
            log::info("ThreadPool initialized successfully with {} workers.",
                      numThreads);

            return pool;

        } catch (const std::system_error &e) {
            return std::unexpected{
                Error::failure(ErrorCode::InternalRuntimeError,
                               fmt, e.what())};
        } catch (...) {
            return std::unexpected{Error::failure(ErrorCode::Unknown,
                                   fmt, "unknown error")};
        }
    }

    ThreadPool::~ThreadPool() noexcept
    {
        {
            std::unique_lock<std::mutex> lock(this->_queueMutex);
            this->_stop = true;

            RTP_VERIFY(this->_tasks.empty(), 
                       "ThreadPool destroyed with {} pending tasks discarded!", 
                       this->_tasks.size());
        }
        this->_condition.notify_all();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Private API
    ///////////////////////////////////////////////////////////////////////////

    void ThreadPool::start(size_t numThreads)
    {
        for (size_t i = 0; i < numThreads; ++i)
            this->_workers.emplace_back([this] (std::stop_token st)
            {
                this->workerThread(st);
            });
    }

    void ThreadPool::workerThread(std::stop_token stopToken) noexcept
    {
        while (true) {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(this->_queueMutex);

                this->_condition.wait(lock, [this](void) {
                    return this->_stop || !this->_tasks.empty();
                });
                if ((stopToken.stop_requested() || this->_stop) &&
                    this->_tasks.empty()) [[unlikely]]
                    break;
                if (this->_tasks.empty()) [[unlikely]]
                    continue;
                RTP_ASSERT(!this->_tasks.empty(), 
                           "Worker: Try to pop from empty queue (Logic Error)");

                task = std::move(this->_tasks.front());
                this->_tasks.pop();
            }

            RTP_ASSERT(task, "Worker: Popped an invalid/null task function!");

            try {
                task();
            } catch (const std::exception &e) {
                log::error("Exception in worker thread: {}", e.what());
            } catch (...) {
                log::error("Unknown exception in worker thread");
            }
        }
    }
}
