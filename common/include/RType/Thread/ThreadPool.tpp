/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ThreadPool.tpp, ThreadPool class implementation of enqueue method
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
 * @file ThreadPool.tpp
 * @brief ThreadPool class template implementation
 * @author Robin Toillon
 * @details This file contains the implementation of the ThreadPool
 * class template methods. Specifically, it implements the enqueue
 * method which allows tasks to be added to the thread pool for
 * asynchronous execution.
 */

#include <format>

namespace rtp::thread
{
    template<std::invocable F, typename... Args>
    auto ThreadPool::enqueue(F &&f, Args &&...args)
        -> std::expected<std::future<std::invoke_result_t<F, Args...>>,
                         rtp::Error>
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
                    return std::unexpected{
                        Error::failure(ErrorCode::InternalRuntimeError,
                                       "enqueue on stopped ThreadPool")};
                this->_tasks.emplace([task = std::move(task)](void) mutable
                                    { task(); });
            }
            this->_condition.notify_one();

            return future;
        } catch (const std::exception &e) {
            return std::unexpected{
                Error::failure(ErrorCode::InternalRuntimeError,
                               "Failed to enqueue task: {}", e.what())};
        } catch (...) {
            return std::unexpected{
                Error::failure(ErrorCode::Unknown,
                               "Failed to enqueue task: Unknown error")};
        }
    }
}
