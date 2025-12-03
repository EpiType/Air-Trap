/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ThreadPool.tpp, ThreadPool class implementation of enqueue method
*/

/**
 * @file ThreadPool.tpp
 * @brief ThreadPool class template implementation
 * @details This file contains the implementation of the ThreadPool class template methods.
 * Specifically, it implements the enqueue method which allows tasks to be added
 * to the thread pool for asynchronous execution.
 */

#include "ThreadPool.hpp"
#include <format>

namespace rtp::thread
{
    template<std::invocable F, typename... Args>
    auto ThreadPool::enqueue(F &&f, Args &&...args) noexcept
        -> std::expected<std::future<std::invoke_result_t<F, Args...>>,
                         std::string>
    {
        using return_t = std::invoke_result_t<F, Args...>;

        try {
            auto boundTask =
                [func = std::forward<F>(f),
                 ...captured_args = std::forward<Args>(args)...](void) mutable
                -> return_t
            {
                return std::invoke(std::move(func),
                                   std::move(captured_args)...);
            };

            std::packaged_task<return_t()> task(std::move(boundTask));
            std::future<return_t> future = task.get_future();
            {
                std::unique_lock<std::mutex> lock(this->_queueMutex);
                if (this->_stop)
                    return std::unexpected{"enqueue on stopped ThreadPool"};
                this->_tasks.emplace([task = std::move(task)](void) mutable
                                    { task(); });
            }
            this->_condition.notify_one();

            return future;
        } catch (const std::exception &e) {
            return std::unexpected{std::format("Failed to enqueue task: {}",
                                               e.what())};
        } catch (...) {
            return std::unexpected{"Failed to enqueue task: Unknown error"};
        }
    }
}