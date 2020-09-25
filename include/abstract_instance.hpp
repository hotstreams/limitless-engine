#pragma once

#include <shader_storage.hpp>
#include <shader_program.hpp>
#include <functional>
#include <material_instance.hpp>

namespace GraphicsEngine {
    class AbstractInstance {
    private:
        static inline uint64_t next_id {0};
        uint64_t id;
    protected:
        ModelShaderType type;
        bool done {false};
        bool hidden {false};
        bool wireframe {false};
    public:
        explicit AbstractInstance(ModelShaderType type) noexcept : id{next_id++}, type{type} { }
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

        using UniformSetter = std::vector<std::function<void(ShaderProgram&)>>;
        virtual void draw(MaterialShaderType shader_type, Blending blending, const UniformSetter& uniform_set = UniformSetter{}) = 0;
    };
}