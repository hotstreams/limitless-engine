#pragma once

#include <condition_variable>
#include <functional>
#include <future>
#include <vector>
#include <thread>
#include <queue>
#include <mutex>

namespace Limitless {
    class ThreadPool {
    protected:
        std::queue<std::function<void()>> tasks;
        std::condition_variable condition;
        std::vector<std::thread> threads;
        std::mutex mutex;
        bool stop{};
        ThreadPool() = default;
    public:
        explicit ThreadPool(uint32_t pool_size);
        virtual ~ThreadPool();

        ThreadPool(const ThreadPool&) = delete;
        ThreadPool& operator=(const ThreadPool&) = delete;

        template<typename F, typename... Args>
        auto add(F&& f, Args&&... args) {
            static_assert(std::is_invocable_v<F&&, Args&&...>);

            // waiting c++2a for generic expansion lambdas
            auto func = [f = std::forward<F>(f), args = std::make_tuple(std::forward<Args>(args)...)] () mutable {
                return std::apply(std::forward<F>(f), std::forward<decltype(args)>(args));
            };

            auto shared_task = std::make_shared<std::packaged_task<std::invoke_result_t<F&&, Args&&...>()>>(std::move(func));
            auto future = shared_task->get_future();

            {
                std::unique_lock lock(mutex);
                tasks.emplace([task = std::move(shared_task)]() mutable { std::invoke(*task); });
            }

            condition.notify_one();

            return future;
        }

        void joinAll();
    };
}
