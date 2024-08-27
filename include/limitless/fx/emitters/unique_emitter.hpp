#pragma once

#include <limitless/fx/emitters/abstract_emitter.hpp>
#include <limitless/models/abstract_mesh.hpp>
#include <limitless/fx/modules/module.hpp>

#include <memory>
#include <set>
#include <limitless/renderer/shader_type.hpp>
#include <optional>

namespace Limitless::ms {
    class Material;
}

namespace Limitless::fx {
    // UniqueEmitterShader determines uniqueness for shader compiler
    class UniqueEmitterShader {
    public:
        // emitter type
        AbstractEmitter::Type emitter_type {};
        // set of modules
        std::set<ModuleType> module_type {};
        // material index
        uint64_t material_type {};
    };

    bool operator<(const UniqueEmitterShader& lhs, const UniqueEmitterShader& rhs) noexcept;
    bool operator==(const UniqueEmitterShader& lhs, const UniqueEmitterShader& rhs) noexcept;

    struct UniqueEmitterShaderKey {
        UniqueEmitterShader emitter_type;
        ShaderType shader;
    };

    bool operator<(const UniqueEmitterShaderKey& lhs, const UniqueEmitterShaderKey& rhs) noexcept;
    bool operator==(const UniqueEmitterShaderKey& lhs, const UniqueEmitterShaderKey& rhs) noexcept;

    // UniqueEmitterRenderer determines uniqueness for emitter draw call
    class UniqueEmitterRenderer {
    public:
        // emitter type
        AbstractEmitter::Type emitter_type {};
        // optional rendering mesh
        std::optional<std::shared_ptr<AbstractMesh>> mesh {};
        // material itself with all prop and uniform values
        std::shared_ptr<ms::Material> material;
    };

    bool operator<(const UniqueEmitterRenderer& lhs, const UniqueEmitterRenderer& rhs) noexcept;
    bool operator==(const UniqueEmitterRenderer& lhs, const UniqueEmitterRenderer& rhs) noexcept;
}