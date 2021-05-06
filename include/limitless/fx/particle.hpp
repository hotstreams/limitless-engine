#pragma once

#include <glm/glm.hpp>

namespace Limitless::fx {
    struct SpriteParticle {
        glm::vec4 color {1.0f};
        // xy - scaling factor; zw - frame uv
        glm::vec4 subUV {1.0f};
        // custom per particle prop materials
        glm::vec4 properties {1.0f};
        glm::vec3 acceleration {0.0f};
        glm::vec3 position {0.0f};
        glm::vec3 rotation {0.0f};
        glm::vec3 velocity {0.0f};
        float lifetime {1.0f};
        // for sprite emitter it is pixel size
        // for mesh emitter it is scaling modifier of specified mesh
        float size {32.0f};
    };

    struct alignas(64) MeshParticle {
        glm::mat4 model {1.0f};
        glm::vec4 color {1.0f};
        glm::vec4 subUV {1.0f};
        glm::vec4 properties {1.0f};
        glm::vec3 acceleration {0.0f};
        glm::vec3 position {0.0f};
        glm::vec3 rotation {0.0f};
        glm::vec3 velocity {0.0f};
        float lifetime {1.0f};
        float size {32.0f};
    };

    struct BeamParticle {
        glm::vec3 position {0.0f};
        glm::vec3 target {2.0f};
        glm::vec4 color {1.0f};
        glm::vec4 subUV {1.0f};
        glm::vec4 properties {1.0f};
        glm::vec3 acceleration {0.0f};
        glm::vec3 rotation {0.0f};
        glm::vec3 velocity {0.0f};
        float lifetime {1.0f};
        float size {5.0f};

        float displacement {0.5f};
        float min_offset {0.1f};
        bool build {false};
        std::chrono::duration<float> rebuild_delta {1.0f};
        std::vector<glm::vec3> derivative_line;
        std::chrono::time_point<std::chrono::steady_clock> last_rebuild {};
    };

    // beam particle representation on GPU for mapping
    struct BeamParticleMapping {
        glm::vec4 position {0.0f};
        glm::vec2 uv {0.0f};
        glm::vec4 color {1.0f};
        glm::vec4 subUV {1.0f};
        glm::vec4 properties {1.0f};
        glm::vec3 acceleration {0.0f};
        glm::vec3 rotation {0.0f};
        glm::vec3 velocity {0.0f};
        float lifetime {1.0f};
        float size {1.0f};
    };
}

