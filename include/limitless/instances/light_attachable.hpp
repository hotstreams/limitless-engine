#pragma once

#include <limitless/lighting/lighting.hpp>
#include <algorithm>
#include <stdexcept>

namespace Limitless {
    class Lighting;

    struct light_attachable_error : std::runtime_error {
        explicit light_attachable_error(const char* error) : runtime_error(error) {}
    };

    class LightAttachable {
    private:
        Lighting* lighting {};

        template<typename T>
        struct LightAttachment {
            uint64_t id;
            glm::vec3 offset;

            LightAttachment(uint64_t _id, const glm::vec3& _offset) noexcept : id{_id}, offset{_offset} {}
        };

        std::vector<LightAttachment<PointLight>> point_lights;
        std::vector<LightAttachment<SpotLight>> spot_lights;
    protected:
        void setPosition(const glm::vec3& position) noexcept;
        void setRotation(const glm::quat& rotation) noexcept;
        void rotateBy(const glm::quat& rotation) noexcept;

        explicit LightAttachable(Lighting* _lighting) noexcept;
        LightAttachable() = default;
        virtual ~LightAttachable();

        LightAttachable(const LightAttachable&) = default;
        LightAttachable(LightAttachable&&) noexcept = default;
    public:
        template<typename T, typename... Args>
        auto attachLight(const glm::vec3& offset, Args&&... args) {
            if (!lighting) {
                throw light_attachable_error("LightAttachable is not initialized properly");
            }

            auto id = static_cast<LightContainer<T>&>(*lighting).emplace_back(std::forward<Args>(args)...);

            if constexpr (std::is_same_v<T, PointLight>)
                point_lights.emplace_back(id, offset);
            else if constexpr (std::is_same_v<T, SpotLight>)
                spot_lights.emplace_back(id, offset);
            else
                static_assert(!std::is_same_v<T, T>, "No such Light Type");

            return id;
        }

        template<typename T>
        auto attachLight(T&& light, const glm::vec3& offset = glm::vec3{0.f, 0.f, 0.f}) {
            if (!lighting) {
                throw light_attachable_error("LightAttachable is not initialized properly");
            }

            auto id = static_cast<LightContainer<T>&>(*lighting).emplace_back(std::forward<T>(light));

            if constexpr (std::is_same_v<T, PointLight>)
                point_lights.emplace_back(id, offset);
            else if constexpr (std::is_same_v<T, SpotLight>)
                spot_lights.emplace_back(id, offset);
            else
                static_assert(!std::is_same_v<T, T>, "No such Light Type");

            return id;
        }

        template<typename T>
        void detachLight(uint64_t id) {
            if (!lighting) {
                throw light_attachable_error("LightAttachable is not initialized properly");
            }

            static_cast<LightContainer<T>&>(*lighting).erase(id);

            auto remove_light_attachment = [&] (auto& container) {
                std::remove_if(container.begin(), container.end(), [&] (auto& light) { return light.id == id; });
            };

            if constexpr (std::is_same_v<T, PointLight>)
                remove_light_attachment(point_lights);
            else if constexpr (std::is_same_v<T, SpotLight>)
                remove_light_attachment(spot_lights);
            else
                static_assert(!std::is_same_v<T, T>, "No such Light Type");
        }
    };
}
