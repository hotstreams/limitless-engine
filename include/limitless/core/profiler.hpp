#pragma once

#include <limitless/core/time_query.hpp>
#include <limitless/text/text_instance.hpp>
#include <map>

namespace Limitless {
    class Profiler final {
    private:
        std::map<std::string, TimeQuery> queries;
    public:
        Profiler() = default;
        ~Profiler() = default;

        void draw(Context& ctx, const Assets& assets);

        TimeQuery& operator[] (const std::string& name);
    };

    inline Profiler profiler;

    class ProfilerScope final {
    private:
        TimeQuery& query;
    public:
        explicit ProfilerScope(const std::string& name);
        ~ProfilerScope();
    };
}
