#pragma once

#include <functional>
#include <cstdint>

namespace GraphicsEngine {
    enum class MaterialShader;
    enum class ModelShader;
    enum class Blending;
    class ShaderProgram;

    class AbstractInstance {
    private:
        static inline uint64_t next_id {0};
        uint64_t id;
    protected:
        ModelShader type;
        bool done {false};
        bool hidden {false};
        bool wireframe {false};

        explicit AbstractInstance(ModelShader type) noexcept : id{next_id++}, type{type} {}
    public:
        virtual ~AbstractInstance() = default;

        [[nodiscard]] const auto& getId() const noexcept { return id; }
        [[nodiscard]] const auto& getType() const noexcept { return type; }

        void reveal() noexcept;
        void hide() noexcept;

        void kill() noexcept;
        [[nodiscard]] bool isKilled() const noexcept;

        void asWireFrame() noexcept;
        void asModel() noexcept;
        [[nodiscard]] bool isWireFrame() const noexcept;

        // draws instance with no extra uniform setting
        virtual void draw(MaterialShader shader_type, Blending blending) = 0;

        using UniformSetter = std::vector<std::function<void(ShaderProgram&)>>;
        virtual void draw(MaterialShader shader_type, Blending blending, const UniformSetter& uniform_set) = 0;
    };
}
