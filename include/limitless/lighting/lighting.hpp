#pragma once

#include <limitless/lighting/light_container.hpp>
#include <limitless/lighting/light.hpp>

namespace Limitless {
    class Context;

    class Lighting final {
    private:
        /**
         *  Internal buffer for scene lighting
         */
        class SceneLighting final {
        public:
            glm::vec4 direction;
            glm::vec4 color;
            glm::vec4 ambient_color;
            uint32_t light_count;
        };

        /**
         *  Scene ambient lighting
         *
         *  rgb - color
         *  a - intensity
         */
        glm::vec4 ambient_color {1.0f, 1.0f, 1.0f, 1.0f};

        /**
         *  Scene directional lighting
         *
         *  scene has only one
         */
        Light directional_light;

        /**
         *  Scene punctual lighting
         *
         *  contains Point and Spot lights
         */
        LightContainer punctual_lights;

        std::shared_ptr<Buffer> buffer;
        Context& context;

        void createLightBuffer();
        void updateSceneLightBuffer();

        bool changed;
    public:
        explicit Lighting(Context& ctx);
        ~Lighting() = default;

        [[nodiscard]] const glm::vec4& getAmbientColor() const noexcept;
        [[nodiscard]] glm::vec4& getAmbientColor() noexcept;
        void setAmbientColor(const glm::vec4& ambient_color) noexcept;

        [[nodiscard]] const Light& getDirectionalLight() const noexcept;
        [[nodiscard]] Light& getDirectionalLight() noexcept;
        void setDirectionalLight(const Light& directional_light);

        [[nodiscard]] bool isChanged() const noexcept;
        void resetChanged() noexcept;
        void change() noexcept;

        [[nodiscard]] const std::map<uint64_t, Light>& getLights() const noexcept;
        [[nodiscard]] std::map<uint64_t, Light>& getLights() noexcept;
        [[nodiscard]] LightContainer& getLightContainer() noexcept;

        Light& add(Light&& light);
        Light& add(const Light& light);

        void update();
    };
}
