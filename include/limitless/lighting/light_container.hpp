#pragma once

#include <map>
#include <unordered_map>
#include <vector>
#include <memory>
#include <limitless/lighting/light.hpp>

namespace Limitless {
    class Buffer;

    class LightContainer {
    private:
        /**
         * Internal light buffer presentation
         */
        class InternalLight {
        private:
            glm::vec4 color;
            glm::vec4 position;
            glm::vec4 direction;
            glm::vec2 scale_offset;
            float falloff;
            uint32_t type;

            static float radiusToFalloff(float r);
            static float falloffToRadius(float f);
            static glm::vec2 anglesToScaleOffset(const glm::vec2& angles);
            static glm::vec2 scaleOffsetToAngles(const glm::vec2& scale_offset);
        public:
            explicit InternalLight(const Light& light) noexcept;
            InternalLight(const InternalLight& light) noexcept = default;

            void update(const Light& light) noexcept;
        };

        std::map<uint64_t, Light> lights;

        // corresponding internal presentation
        std::map<uint64_t, InternalLight> internal_lights;

        // lights to be mapped into the buffer
        std::vector<InternalLight> visible_lights;

        // visible lights buffer
        std::shared_ptr<Buffer> buffer;
    public:
        LightContainer();
        ~LightContainer() = default;

        [[nodiscard]] auto& getLights() noexcept { return lights; }
        [[nodiscard]] const auto& getLights() const noexcept { return lights; }
        [[nodiscard]] uint64_t size() const noexcept { return lights.size(); }
        [[nodiscard]] uint64_t visibleSize() const noexcept { return visible_lights.size(); }

        Light& add(Light&& light);
        Light& add(const Light& light);

        void update();
    };
}
