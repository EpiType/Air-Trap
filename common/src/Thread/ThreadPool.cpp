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

#include "ThreadPool.hpp"
#include "RType/Core/Logger/Logger.hpp"

#include <format>

namespace rtp::thread
{

    ///////////////////////////////////////////////////////////////////////////
    // Public API
    ///////////////////////////////////////////////////////////////////////////

    auto ThreadPool::create(size_t numThreads) noexcept
        -> std::expected<std::unique_ptr<ThreadPool>, std::string>
    {
        if (numThreads == 0)
            return std::unexpected{
                "ThreadPool must have at least one thread."};

        std::unique_ptr<ThreadPool> pool{new (std::nothrow) ThreadPool{}};
        if (!pool)
            return std::unexpected{
                "ThreadPool init failed: memory allocation error"};
        try {
            log::info("ThreadPool initialized successfully with {} workers.",
                      numThreads);
            pool->start(numThreads);
        } catch (const std::exception &e) {
            std::string err{std::format("ThreadPool init failed: {}",
                                          e.what())};
            log::error("{}", err);
            return std::unexpected{err};
        } catch (...) {
            std::string err{"ThreadPool init failed: unknown error"};
            return std::unexpected{err};
        }

        return pool;
    }

    ThreadPool::~ThreadPool() noexcept
    {
        {
            std::unique_lock<std::mutex> lock(this->_queueMutex);
            this->_stop = true;
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

                this->_condition.wait(lock, [this, &stopToken] (void) {
                    return this->_stop || !this->_tasks.empty();
                });
                if ((stopToken.stop_requested() || this->_stop) &&
                    this->_tasks.empty())
                    break;
                if (this->_tasks.empty())
                    continue;

                task = std::move(this->_tasks.front());
                this->_tasks.pop();
            }
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
