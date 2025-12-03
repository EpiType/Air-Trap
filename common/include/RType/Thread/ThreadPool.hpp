/*
** EPITECH PROJECT, 2025
** R-Type
** File description:
** ThreadPool.hpp, ThreadPool class declaration
*/

/**
 * @file ThreadPool.hpp
 * @brief ThreadPool class declaration
 * 
 * @details This file contains the declaration of the ThreadPool class.
 * The class allows tasks to be enqueued and executed concurrently
 * by a pool of worker threads.
 * It is designed to be thread-safe by leveraging mutexes and
 * condition variables for task management and synchronization.
 */

#ifndef THREADPOOL_HPP_
    #define THREADPOOL_HPP_
    
    #include <concepts>
    #include <condition_variable>
    #include <expected>
    #include <functional>
    #include <memory>
    #include <future>
    #include <mutex>
    #include <queue>
    #include <thread>
    #include <vector>

namespace rtp::thread
{
    /**
     * @class ThreadPool
     * @brief ThreadPool class for managing a pool of threads
     *
     * @details The ThreadPool class creates a fixed number of worker threads at construction.
     * Tasks are enqueued to be executed asynchronously, with each worker thread continuously
     * fetching and executing tasks from a shared queue.
     * Synchronization of accesses to the task queue is maintained
     * with a mutex and a condition variable.
     * The destructor takes care of shutting down the worker threads gracefully
     * to ensure proper cleanup of resources.
     */
    class ThreadPool final {
        public:
            ThreadPool(const ThreadPool &) = delete;
            ThreadPool &operator=(const ThreadPool &) = delete;

            /**
             * @brief Creates a ThreadPool with the specified number of threads.
             *
             * @param numThreads The number of worker threads to create in the pool.
             * @return std::expected<std::unique_ptr<ThreadPool>, std::string>
             *         A unique pointer to the created ThreadPool on success,
             *         or an error message on failure.
             *
             * @details This static method constructs a ThreadPool instance
             * with the given number of threads.
             * It validates the input to ensure that at least one thread is created
             * and that the number of threads does not exceed the hardware concurrency.
             * If the input is invalid, an appropriate error message is returned.
             */
            [[nodiscard]]
            static auto create(size_t numThreads) noexcept
                -> std::expected<std::unique_ptr<ThreadPool>, std::string>;

            /**
             * @brief Destroys the ThreadPool,
             * ensuring all threads are joined and resources are released.
             *
             * @details Upon destruction,
             * the ThreadPool stops accepting new tasks and waits for all currently
             * queued tasks to finish execution before joining the worker threads.
             */
            ~ThreadPool() noexcept;

            /**
             * @brief Enqueues a task for asynchronous execution.
             *
             * @tparam F Type of the callable task.
             * @tparam Args Types of the arguments to be passed to the callable.
             * @param f The callable object to be executed.
             * @param args Arguments to be passed to the callable object.
             * @return A std::future representing the result of the asynchronous task.
             *
             * @details The task is wrapped into a std::move_only_function<void(void)> and
             * then stored in the queue.
             * A condition variable is notified to wake up one of the waiting worker threads.
             */
            template<std::invocable F, typename... Args>
            [[nodiscard]]
            auto enqueue(F &&f, Args &&...args) noexcept
                -> std::expected<std::future<std::invoke_result_t<F, Args...>>,
                                 std::string>;

        private:
            std::vector<std::jthread> _workers; /**< Vector of worker threads */
            std::queue<std::move_only_function<void(void)>> _tasks; /**< Queue of tasks to be executed */
            std::mutex _queueMutex; /**< Mutex for synchronizing access to the task queue */
            std::condition_variable _condition; /**< Condition variable for notifying worker threads */
            bool _stop{false}; /**< Flag indicating whether the ThreadPool is stopping */

        private:
            /**
             * @brief Private constructor to initialize the ThreadPool.
             *
             * @details The constructor is private to enforce the use of the static
             * create method for instantiation.
             * It initializes the ThreadPool with the specified number of threads.
             */
            ThreadPool(void) noexcept = default;

            /**
             * @brief Starts the worker threads in the pool.
             *
             * @param numThreads The number of worker threads to create.
             *
             * @details This method initializes the specified number of worker threads.
             * Each thread runs the workerThread function,
             * which continuously processes tasks from the queue.
             */
            void start(size_t numThreads);

            /**
             * @brief The worker thread function that continuously processes tasks.
             *
             * @details Each worker thread executes this function in a loop.
             * The function waits for a new task to become available.
             * When notified or when a task is enqueued, the thread locks the queue,
             * retrieves the task, unlocks the queue, and then executes the task.
             * It continues this cycle until the ThreadPool is signaled to stop.
             */
            void workerThread(std::stop_token stopToken) noexcept;

    };
}

    #include "ThreadPool.tpp" /* Enqueue method implementation */

#endif /* !THREADPOOL_HPP_ */