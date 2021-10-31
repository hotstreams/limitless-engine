#pragma once

#include <string>
#include <glm/glm.hpp>
#include <array>

namespace Limitless {
    struct Bone {
        std::string name;
        glm::mat4 node_transform {1.0f};
        glm::mat4 offset_matrix {1.0f};

        Bone(std::string name, const glm::mat4& _offset_matrix) noexcept
            : name{std::move(name)}
            , offset_matrix {_offset_matrix} {
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
