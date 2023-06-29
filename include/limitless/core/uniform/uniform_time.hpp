#pragma once

#include <limitless/core/texture_visitor.hpp>
#include <limitless/core/context_debug.hpp>

#include <memory>
#include <string>
#include <chrono>

#include <limitless/core/uniform/uniform_value.hpp>

namespace Limitless {
    class Texture;
    class ShaderProgram;

    class UniformTime : public UniformValue<float> {
    private:
        std::chrono::time_point<std::chrono::steady_clock> start;
        friend class UniformSerializer;
    public:
        explicit UniformTime(const std::string& name) noexcept;
        ~UniformTime() override = default;

        void update() noexcept;

        [[nodiscard]] bool& getChanged() noexcept override { changed = true; return changed; }

        [[nodiscard]] UniformTime* clone() noexcept override;
        void set(const ShaderProgram& shader) override;
    };
}
