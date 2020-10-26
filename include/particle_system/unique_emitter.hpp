#pragma once

#include <particle_system/emitter.hpp>

namespace GraphicsEngine {
    class Material;

    struct UniqueEmitter {
        std::vector<EmitterModuleType> modules;
    };

    struct UniqueSpriteEmitter : public UniqueEmitter {
        std::shared_ptr<Material> material;
    };

    struct UniqueMeshEmitter : public UniqueSpriteEmitter {
        std::shared_ptr<AbstractMesh> mesh;
    };

    bool operator<(const UniqueSpriteEmitter& lhs, const UniqueSpriteEmitter& rhs) noexcept;
    bool operator<(const UniqueMeshEmitter& lhs, const UniqueMeshEmitter& rhs) noexcept;

    bool operator==(const UniqueSpriteEmitter& lhs, const UniqueSpriteEmitter& rhs) noexcept;
    bool operator==(const UniqueMeshEmitter& lhs, const UniqueMeshEmitter& rhs) noexcept;
}