#include <core/context_thread_pool.hpp>

using namespace GraphicsEngine;

ContextThreadPool::ContextThreadPool(Context& shared, uint32_t pool_size) : ThreadPool{} {
    for (uint32_t i = 0; i < pool_size; ++i) {
        context_workers.emplace_back("thread_worker", glm::uvec2{1, 1}, shared, WindowHints{{WindowHint::Visible, false}});

        auto lambda = [&, i] {
            context_workers[i].makeCurrent();

            for (;;) {
                std::function<void()> task;

                {
                    std::unique_lock lock(mutex);

                    condition.wait(lock, [&] () { return stop || !tasks.empty(); });

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
