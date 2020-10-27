#pragma once

#include <util/thread_pool.hpp>
#include <core/context.hpp>

namespace GraphicsEngine {
    class ContextThreadPool : public ThreadPool {
    private:
        std::vector<Context> context_workers;
    public:
        explicit ContextThreadPool(Context& shared, uint32_t pool_size = std::thread::hardware_concurrency());
        ~ContextThreadPool() override = default;
    };
}