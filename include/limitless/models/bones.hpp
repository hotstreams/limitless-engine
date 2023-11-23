#pragma once

#include <string>
#include <glm/glm.hpp>
#include <array>
#include <glm/gtc/quaternion.hpp>
#include <optional>

namespace Limitless {
    struct Bone {
        std::optional<uint32_t> joint_index;
        uint32_t index;
        std::string name;
        glm::mat4 node_transform {1.0f};
        glm::mat4 offset_matrix {1.0f};

        // Bone node local transforms as individual components.
        glm::vec3 position {0.f};
        glm::fquat rotation {1.f, 0.f, 0.f, 0.f};
        glm::vec3 scale {1.f};

        Bone(uint32_t _index, std::string name, const glm::mat4& _offset_matrix) noexcept
            : index {_index}
            , name{std::move(name)}
            , offset_matrix {_offset_matrix} {
        }

        Bone(
            uint32_t _index,
            std::string _name,
            const glm::mat4& _node_transform,
            const glm::mat4& _offset_matrix
        )
            : index {_index}
            , name {std::move(_name)}
            , node_transform {_node_transform}
            , offset_matrix {_offset_matrix}
        {
        }

        [[nodiscard]] auto isFake() const noexcept { return name.at(0) == '<'; }
    };

    struct VertexBoneWeight {
        static constexpr uint32_t BONE_COUNT = 4;

        std::array<uint32_t, BONE_COUNT> bone_index;
        std::array<float, BONE_COUNT> weight;

        void addBoneWeight(uint32_t id, float w) noexcept {
            uint32_t i;
            for (i = 0; i < BONE_COUNT && weight[i] != 0.0f; ++i);

            if (i >= BONE_COUNT) {/* no more weight slots */ return; }

            bone_index[i] = id;
            weight[i] = w;
        }
    };
}
