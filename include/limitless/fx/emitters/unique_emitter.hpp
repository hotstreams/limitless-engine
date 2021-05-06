#pragma once

#include <limitless/fx/emitters/abstract_emitter.hpp>
#include <limitless/fx/modules/module.hpp>

#include <set>
#include <limitless/pipeline/shader_pass_types.hpp>

namespace Limitless::fx {
    class UniqueEmitter {
    public:
        // emitter type
        AbstractEmitter::Type emitter_type;
        // set of modules
        std::set<ModuleType> module_type;
        // material index
        uint64_t material_type;
    };

    bool operator<(const UniqueEmitter& lhs, const UniqueEmitter& rhs) noexcept;
    bool operator==(const UniqueEmitter& lhs, const UniqueEmitter& rhs) noexcept;

    struct UniqueEmitterShaderKey {
        UniqueEmitter emitter_type;
        ShaderPass shader;
    };

    bool operator<(const UniqueEmitterShaderKey& lhs, const UniqueEmitterShaderKey& rhs) noexcept;
    bool operator==(const UniqueEmitterShaderKey& lhs, const UniqueEmitterShaderKey& rhs) noexcept;
}