#include <limitless/core/context_thread_pool.hpp>

using namespace Limitless;

ContextThreadPool::ContextThreadPool(Context& shared, uint32_t pool_size)
    : ThreadPool() {
    for (uint32_t i = 0; i < pool_size; ++i) {
        context_workers.emplace_back(
            Context::builder()
                    .title("thread_worker")
                    .size(glm::uvec2{1, 1})
                    .not_visible()
                    .shared(&shared)
                    .build()
        );

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

                // explicitly share state between contexts
                {
                    // make sure that all commands in a queue
                    glFlush();

                    // make sure that all commands are finished
                    glFinish();
                }
            }
        };

        threads.emplace_back(std::move(lambda));
    }
}
