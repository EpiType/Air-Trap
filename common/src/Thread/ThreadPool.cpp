/*
** EPITECH PROJECT, 2025
** The Plazza
** File description:
** ThreadPool.cpp, ThreadPool class implementation
*/

/**
 * @file ThreadPool.cpp
 * @brief ThreadPool class implementation
 * @details This file contains the implementation of the ThreadPool class.
 * The ThreadPool class manages a pool of worker threads that can execute
 * tasks asynchronously. Tasks can be enqueued to the pool, and the worker threads
 * will process them concurrently. The class ensures thread-safe access to
 * the task queue using mutexes and condition variables.
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
        -> std::expected<std::unique_ptr<ThreadPool>, std::string>
    {
        constexpr std::string_view fmt{"ThreadPool init failed: {}"};

        if (numThreads == 0)
            return std::unexpected{std::format(
                fmt, "number of threads must be at least 1")};

        try {
            std::unique_ptr<ThreadPool> pool{new ThreadPool{}};

            pool->start(numThreads);
            log::info("ThreadPool initialized successfully with {} workers.",
                      numThreads);

            return pool;

        } catch (const std::system_error &e) {
            return std::unexpected{std::format(fmt, e.what())};
        } catch (...) {
            return std::unexpected{std::format(fmt, "unknown error")};
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
            std::move_only_function<void(void)> task;
            {
                std::unique_lock<std::mutex> lock(this->_queueMutex);

                this->_condition.wait(lock, [this, &stopToken](void) {
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
