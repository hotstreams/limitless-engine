#pragma once

#include <emitter_renderer.hpp>

namespace GraphicsEngine {
    class SpriteEmitterRenderer;
    class MeshEmitterRenderer;

    class EmitterVisiter;

    class AbstractInstance;
    enum class Blending;

    class EffectRenderer {
    private:
        std::map<UniqueSpriteEmitter, SpriteEmitterRenderer> sprite_renderer;
        std::map<UniqueMeshEmitter, MeshEmitterRenderer> mesh_renderer;

        void addStorage(const std::vector<AbstractInstance*>& instances) noexcept;
        static void visitEmitters(const std::vector<AbstractInstance*>& instances, EmitterVisiter& visiter) noexcept;
    public:
        explicit EffectRenderer(Context& context) noexcept;
        ~EffectRenderer() = default;

        void update(const std::vector<AbstractInstance*>& instances);
        void draw(Blending blending);
    };
}