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
        glm::vec4 acceleration_lifetime {0.0f, 0.0f, 0.0f, 1.0f};
        // xyz - position; w - size
        glm::vec4 position_size {0.0f, 0.0f, 0.0f, 32.0f};
        // xyz - rotation; w - time;
        glm::vec4 rotation_time {0.0f};
        // xyz - velocity; w -
        glm::vec4 velocity {0.0f};

        auto& getColor() noexcept { return color; }
        auto& getSubUV() noexcept { return subUV; }
        auto& getCustomProperties() noexcept { return properties; }
        auto& getAcceleration() noexcept { return reinterpret_cast<glm::vec3&>(acceleration_lifetime); }
        auto& getLifetime() noexcept { return acceleration_lifetime.w; }
        auto& getPosition() noexcept { return reinterpret_cast<glm::vec3&>(position_size); }
        auto& getSize() noexcept { return position_size.w; }
        auto& getRotation() noexcept { return reinterpret_cast<glm::vec3&>(rotation_time); }
        auto& getTime() noexcept { return rotation_time.w; }
        auto& getVelocity() noexcept  { return reinterpret_cast<glm::vec3&>(velocity); }

        const auto& getColor() const noexcept { return color; }
        const auto& getSubUV() const noexcept { return subUV; }
        const auto& getCustomProperties() const noexcept { return properties; }
        const auto& getAcceleration() const noexcept { return reinterpret_cast<const glm::vec3&>(acceleration_lifetime); }
        const auto& getLifetime() const noexcept { return acceleration_lifetime.w; }
        const auto& getPosition() const noexcept { return reinterpret_cast<const glm::vec3&>(position_size); }
        const auto& getSize() const noexcept { return position_size.w; }
        const auto& getRotation() const noexcept { return reinterpret_cast<const glm::vec3&>(rotation_time); }
        const auto& getTime() const noexcept { return rotation_time.w; }
        const auto& getVelocity() const noexcept  { return reinterpret_cast<const glm::vec3&>(velocity); }
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
        glm::vec4 acceleration_lifetime {0.0f, 0.0f, 0.0f, 1.0f};
        // xyz - position; w -
        glm::vec4 position {0.0f};
        // xyz - rotation; w - time;
        glm::vec4 rotation_time {0.0f};
        // xyz - velocity; w -
        glm::vec4 velocity {0.0f};
        // xyz - size; w -
        glm::vec4 size {1.0f, 1.0f, 1.0f, 0.0f};

        auto& getColor() noexcept { return color; }
        auto& getSubUV() noexcept { return subUV; }
        auto& getCustomProperties() noexcept { return properties; }
        auto& getAcceleration() noexcept { return reinterpret_cast<glm::vec3&>(acceleration_lifetime); }
        auto& getLifetime() noexcept { return acceleration_lifetime.w; }
        auto& getPosition() noexcept { return reinterpret_cast<glm::vec3&>(position); }
        auto& getSize() noexcept { return reinterpret_cast<glm::vec3&>(size); }
        auto& getRotation() noexcept { return reinterpret_cast<glm::vec3&>(rotation_time); }
        auto& getTime() noexcept { return rotation_time.w; }
        auto& getVelocity() noexcept  { return reinterpret_cast<glm::vec3&>(velocity); }

        const auto& getColor() const noexcept { return color; }
        const auto& getSubUV() const noexcept { return subUV; }
        const auto& getCustomProperties() const noexcept { return properties; }
        const auto& getAcceleration() const noexcept { return reinterpret_cast<const glm::vec3&>(acceleration_lifetime); }
        const auto& getLifetime() const noexcept { return acceleration_lifetime.w; }
        const auto& getPosition() const noexcept { return reinterpret_cast<const glm::vec3&>(position); }
        const auto& getSize() const noexcept { return reinterpret_cast<const glm::vec3&>(size); }
        const auto& getRotation() const noexcept { return reinterpret_cast<const glm::vec3&>(rotation_time); }
        const auto& getTime() const noexcept { return rotation_time.w; }
        const auto& getVelocity() const noexcept  { return reinterpret_cast<const glm::vec3&>(velocity); }
    };

    struct BeamParticle {
        // vec4 color
        glm::vec4 color {1.0f};
        // xy - scaling factor; zw - frame uv
        glm::vec4 subUV {1.0f};
        // custom 4 float properties per particle
        glm::vec4 properties {1.0f};
        // xyz - acceleration; w - lifetime
        glm::vec4 acceleration_lifetime {0.0f, 0.0f, 0.0f, 1.0f};
        // xyz - position; w - size
        glm::vec4 position_size {0.0f, 0.0f, 0.0f, 16.0f};
        // xyz - rotation; w - time;
        glm::vec4 rotation_time {0.0f};
        // xyz - velocity; w - displacement
        glm::vec4 velocity_displacement {0.0f, 0.0f, 0.0f, 0.5f};
        // xyz - target; w - min_offset
        glm::vec4 target_offset {2.0f, 2.0f, 2.0f, 0.1f};
        // x - speed; y - current length of the beam used in SpeedModulo; zw - unused
        glm::vec4 speed_length;

        bool build {};

        std::chrono::time_point<std::chrono::steady_clock> speed_start {};

        std::chrono::duration<float> rebuild_delta {1.0f};
        std::vector<glm::vec3> derivative_line;
        std::chrono::time_point<std::chrono::steady_clock> last_rebuild {};

        auto& getColor() noexcept { return color; }
        auto& getSubUV() noexcept { return subUV; }
        auto& getCustomProperties() noexcept { return properties; }
        auto& getAcceleration() noexcept { return reinterpret_cast<glm::vec3&>(acceleration_lifetime); }
        auto& getLifetime() noexcept { return acceleration_lifetime.w; }
        auto& getPosition() noexcept { return reinterpret_cast<glm::vec3&>(position_size); }
        auto& getSize() noexcept { return position_size.w; }
        auto& getRotation() noexcept { return reinterpret_cast<glm::vec3&>(rotation_time); }
        auto& getTime() noexcept { return rotation_time.w; }
        auto& getVelocity() noexcept  { return reinterpret_cast<glm::vec3&>(velocity_displacement); }
        auto& getDisplacement() noexcept { return velocity_displacement.w; }
        auto& getTarget() noexcept { return reinterpret_cast<glm::vec3&>(target_offset); }
        auto& getMinOffset() noexcept { return target_offset.w; }
        auto& getLength() noexcept { return speed_length.y; }
        auto& getSpeed() noexcept { return speed_length.x; }

        const auto& getColor() const noexcept { return color; }
        const auto& getSubUV() const noexcept { return subUV; }
        const auto& getCustomProperties() const noexcept { return properties; }
        const auto& getAcceleration() const noexcept { return reinterpret_cast<const glm::vec3&>(acceleration_lifetime); }
        const auto& getLifetime() const noexcept { return acceleration_lifetime.w; }
        const auto& getPosition() const noexcept { return reinterpret_cast<const glm::vec3&>(position_size); }
        const auto& getSize() const noexcept { return position_size.w; }
        const auto& getRotation() const noexcept { return reinterpret_cast<const glm::vec3&>(rotation_time); }
        const auto& getTime() const noexcept { return rotation_time.w; }
        const auto& getVelocity() const noexcept  { return reinterpret_cast<const glm::vec3&>(velocity_displacement); }
        const auto& getDisplacement() const noexcept { return velocity_displacement.w; }
        const auto& getTarget() const noexcept { return reinterpret_cast<const glm::vec3&>(target_offset); }
        const auto& getMinOffset() const noexcept { return target_offset.w; }
        const auto& getLength() const noexcept { return speed_length.y; }
        const auto& getSpeed() const noexcept { return speed_length.x; }
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
        glm::vec4 acceleration_lifetime {0.0f, 0.0f, 0.0f, 1.0f};
        // xyz - rotation; w - time;
        glm::vec4 rotation_time {0.0f};
        // xyz - velocity; w - size
        glm::vec4 velocity_size {0.0f, 0.0f, 0.0f, 1.0f};
        // xy - uv; z - length; w - unused
        glm::vec4 uv_length {0.0f};
        glm::vec3 start {};
        glm::vec3 end {};

        auto& getColor() noexcept { return color; }
        auto& getSubUV() noexcept { return subUV; }
        auto& getCustomProperties() noexcept { return properties; }
        auto& getAcceleration() noexcept { return reinterpret_cast<glm::vec3&>(acceleration_lifetime); }
        auto& getLifetime() noexcept { return acceleration_lifetime.w; }
        auto& getPosition() noexcept { return position; }
        auto& getSize() noexcept { return velocity_size.w; }
        auto& getRotation() noexcept { return reinterpret_cast<glm::vec3&>(rotation_time); }
        auto& getTime() noexcept { return rotation_time.w; }
        auto& getVelocity() noexcept  { return reinterpret_cast<glm::vec3&>(velocity_size); }
        auto& getUV() noexcept { return reinterpret_cast<glm::vec2&>(uv_length); }
        auto& getLength() noexcept { return uv_length.z; }
        auto& getStart() noexcept { return start; }
        auto& getEnd() noexcept { return end; }

        const auto& getColor() const noexcept { return color; }
        const auto& getSubUV() const noexcept { return subUV; }
        const auto& getCustomProperties() const noexcept { return properties; }
        const auto& getAcceleration() const noexcept { return reinterpret_cast<const glm::vec3&>(acceleration_lifetime); }
        const auto& getLifetime() const noexcept { return acceleration_lifetime.w; }
        const auto& getPosition() const noexcept { return position; }
        const auto& getSize() const noexcept { return velocity_size.w; }
        const auto& getRotation() const noexcept { return reinterpret_cast<const glm::vec3&>(rotation_time); }
        const auto& getTime() const noexcept { return rotation_time.w; }
        const auto& getVelocity() const noexcept  { return reinterpret_cast<const glm::vec3&>(velocity_size); }
        const auto& getUV() const noexcept { return reinterpret_cast<const glm::vec2&>(uv_length); }
        const auto& getLength() const noexcept { return uv_length.z; }
        const auto& getStart() const noexcept { return start; }
        const auto& getEnd() const noexcept { return end; }
    };

    VertexArray& operator<<(VertexArray& vertex_array, const std::pair<SpriteParticle, const std::shared_ptr<Buffer>&>& attribute) noexcept;
    VertexArray& operator<<(VertexArray& vertex_array, const std::pair<BeamParticleMapping, const std::shared_ptr<Buffer>&>& attribute) noexcept;
}

