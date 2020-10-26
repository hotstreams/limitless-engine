#pragma once

#include <glm/glm.hpp>

namespace GraphicsEngine {
    struct Particle {
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
       glm::mat4 model {};
       Particle particle;
    };
}

