#pragma once

#include <limitless/core/uniform/uniform_value.hpp>

namespace Limitless {
    /**
     * Describes uniform that stores monotonic clock value in shader program
     */
    class UniformTime : public UniformValue<float> {
    private:
        std::chrono::time_point<std::chrono::steady_clock> start;
        friend class UniformSerializer;

        void update() noexcept;
    public:
        explicit UniformTime(std::string name, GLint location) noexcept;
        ~UniformTime() override = default;

        [[nodiscard]] std::unique_ptr<Uniform> clone() noexcept override;
        void set() override;

        void reset() noexcept;
    };
}
