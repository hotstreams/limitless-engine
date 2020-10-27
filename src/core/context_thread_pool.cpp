#include <core/context_thread_pool.hpp>
#include <iostream>

using namespace GraphicsEngine;

ContextThreadPool::ContextThreadPool(Context& shared, uint32_t pool_size) : ThreadPool{} {
    context_workers.reserve(pool_size);
    for (uint32_t i = 0; i < pool_size; ++i) {
        context_workers.emplace_back("thread_worker", glm::uvec2{1, 1}, shared, WindowHints{{WindowHint::Visible, false}});

        auto lambda = [this, i] {
            context_workers[i].makeCurrent();

            for (;;) {
                context_workers[i].makeCurrent();

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
