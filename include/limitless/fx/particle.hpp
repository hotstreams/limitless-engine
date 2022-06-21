#pragma once

#include <glm/glm.hpp>
#include <chrono>
#include <vector>
#include <memory>

namespace Limitless {
    class VertexArray;
    class Buffer;
}

namespace Limitless::fx {
    struct SpriteParticle {
        // vec4 color
        glm::vec4 color {1.0f};
        // xy - scaling factor; zw - frame uv
        glm::vec4 subUV {1.0f};
        // custom 4 float properties per particle
        glm::vec4 properties {1.0f};
        // xyz - acceleration; w - lifetime
        glm::vec3 acceleration {0.0f};
        float lifetime {1.0f};
        // xyz - position; w - size
        glm::vec3 position {0.0f};
        float size {32.0f};
        // xyz - rotation; w - time;
        glm::vec3 rotation {0.0f};
        float time {0.0f};
        // xyz - velocity; w -
        glm::vec3 velocity {0.0f};
        float _pad {};
    };

    struct alignas(64) MeshParticle {
        glm::mat4 model;
        // vec4 color
        glm::vec4 color {1.0f};
        // xy - scaling factor; zw - frame uv
        glm::vec4 subUV {1.0f};
        // custom 4 float properties per particle
        glm::vec4 properties {1.0f};
        // xyz - acceleration; w - lifetime
        glm::vec3 acceleration {0.0f};
        float lifetime {1.0f};
        // xyz - position; w -
        glm::vec3 position {0.0f};
        float _pad0 {};
        // xyz - rotation; w - time;
        glm::vec3 rotation {0.0f};
        float time {0.0f};
        // xyz - velocity; w -
        glm::vec3 velocity {0.0f};
        float _pad1 {};
        // xyz - size; w -
        glm::vec3 size {1.0f, 1.0f, 1.0f};
        float _pad2 {};
    };

    struct BeamParticle {
        // vec4 color
        glm::vec4 color {1.0f};
        // xy - scaling factor; zw - frame uv
        glm::vec4 subUV {1.0f};
        // custom 4 float properties per particle
        glm::vec4 properties {1.0f};
        // xyz - acceleration; w - lifetime
        glm::vec3 acceleration {0.0f};
        float lifetime {1.0f};
        // xyz - position; w - size
        glm::vec3 position {0.0f, 0.0f, 0.0f};
        float size {16.0f};
        // xyz - rotation; w - time;
        glm::vec3 rotation {0.0f};
        float time {0.0f};
        // xyz - velocity; w - displacement
        glm::vec3 velocity {0.0f};
        float displacement {0.5f};
        // xyz - target; w - min_offset
        glm::vec3 target {2.0f};
        float offset {0.1f};
        // x - speed; y - current length of the beam used in SpeedModulo; zw - unused
        float speed {};
        float length {};

        bool build {};

        std::chrono::time_point<std::chrono::steady_clock> speed_start {};

        std::chrono::duration<float> rebuild_delta {1.0f};
        std::vector<glm::vec3> derivative_line;
        std::chrono::time_point<std::chrono::steady_clock> last_rebuild {};
    };

    // beam particle representation on GPU for mapping
    struct BeamParticleMapping {
        // xyzw - position
        glm::vec4 position {0.0f};
        // vec4 color
        glm::vec4 color {1.0f};
        // xy - scaling factor; zw - frame uv
        glm::vec4 subUV {1.0f};
        // custom 4 float properties per particle
        glm::vec4 properties {1.0f};
        // xyz - acceleration; w - lifetime
        glm::vec3 acceleration {0.0f};
        float lifetime {1.0f};
        // xyz - rotation; w - time;
        glm::vec3 rotation {0.0f};
        float time {0.0f};
        // xyz - velocity; w - size
        glm::vec3 velocity {0.0f};
        float size {1.0f};
        // xy - uv; z - length; w - unused
        glm::vec2 uv {0.0f};
        float length {0.0f};
        float _pad0 {};
        glm::vec3 start {};
        glm::vec3 end {};
    };

    VertexArray& operator<<(VertexArray& vertex_array, const std::pair<SpriteParticle, const std::shared_ptr<Buffer>&>& attribute) noexcept;
    VertexArray& operator<<(VertexArray& vertex_array, const std::pair<BeamParticleMapping, const std::shared_ptr<Buffer>&>& attribute) noexcept;
}

