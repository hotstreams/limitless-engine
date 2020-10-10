#pragma once

#include <glm/glm.hpp>

namespace GraphicsEngine {
    struct Particle {
        glm::vec3 position {0.0f};
        glm::vec4 color {1.0f};
        glm::vec3 angle {0.0f};
        glm::vec3 velocity {0.0f};
        glm::vec3 acceleration {0.0f};
        float lifetime {1.0f};
        float size {256.0f};
        // xy - scaling factor; zw - frame uv
        glm::vec4 subUV {1.0f};
        // custom per particle prop materials
        glm::vec4 properties {1.0f};
    };

    struct MeshParticle {
        glm::mat4 model;
        glm::vec4 color;
        glm::vec4 subUV;
        glm::vec4 properties;
    };
}

