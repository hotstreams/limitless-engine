#include <limitless/util/thread_pool.hpp>

using namespace Limitless;

ThreadPool::ThreadPool(uint32_t pool_size) {
    for (uint32_t i = 0; i < pool_size; ++i) {
        auto lambda = [this] {
            for (;;) {
                std::function<void()> task;

                {
                    std::unique_lock lock(mutex);

                    condition.wait(lock, [this] () { return stop || !tasks.empty(); });

                    if (stop && tasks.empty()) return;

                    task = std::move(tasks.front());
                    tasks.pop();
                }

                task();
            }
        };

        threads.emplace_back(std::move(lambda));
    }
}

void ThreadPool::joinAll() {
    {
        std::unique_lock lock(mutex);
        stop = true;
    }

    condition.notify_all();

    for (auto& thread : threads) {
        thread.join();
    }
}

ThreadPool::~ThreadPool() {
    joinAll();
}