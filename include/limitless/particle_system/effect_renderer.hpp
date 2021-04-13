#pragma once

#include <limitless/particle_system/emitter_renderer.hpp>
#include <limitless/shader_types.hpp>

namespace LimitlessEngine {
    class SpriteEmitterRenderer;
    class MeshEmitterRenderer;

    class UniformSetter;

    class EmitterVisitor;

    class AbstractInstance;
    enum class Blending;

    class Context;

    class EffectRenderer {
    private:
        std::map<UniqueSpriteEmitter, SpriteEmitterRenderer> sprite_renderer;
        std::map<UniqueMeshEmitter, MeshEmitterRenderer> mesh_renderer;

        void addStorage(const std::vector<AbstractInstance*>& instances) noexcept;
        static void visitEmitters(const std::vector<AbstractInstance*>& instances, EmitterVisitor& visitor) noexcept;
    public:
        explicit EffectRenderer(Context& context) noexcept;
        ~EffectRenderer() = default;

        void update(const std::vector<AbstractInstance*>& instances);
        void draw(const Assets& assets, MaterialShader shader, Blending blending, const UniformSetter& setter);
    };
}