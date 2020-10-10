#pragma once

#include <buffer_builder.hpp>

namespace GraphicsEngine {
    struct PointLight {
        glm::vec4 position;
        glm::vec4 color;
        float constant {1.0f};
        float linear {0.7f};
        float quadratic {1.8f};
        float radius;

        PointLight() noexcept = default;

        PointLight(const glm::vec3& _position, const glm::vec4& _color, float _radius) noexcept
            : position{_position, 1.0f}, color{_color}, radius{_radius} {}
    };

    struct DirectionalLight {
        glm::vec4 direction;
        glm::vec4 color;
    };

    template<typename T>
    class Light {
    private:
        uint64_t next_id {0};
        std::vector<T> lights;
        std::unordered_map<uint64_t, uint64_t> lights_map;
        bool modified {false};

        std::shared_ptr<Buffer> light_buffer;

        static constexpr uint32_t max_count = 16;
    public:
        explicit Light(const std::string& name) {
            std::vector<T> data(max_count);
            light_buffer = BufferBuilder::buildIndexed(name, Buffer::Type::ShaderStorage, data, Buffer::Usage::DynamicDraw, Buffer::MutableAccess::WriteOrphaning);
        }

        T& get(uint64_t id) const {
            try {
                return lights.at(lights_map.at(id));
            } catch(const std::out_of_range& e) {
                throw std::runtime_error("Failed to get light, no such id.");
            }
        }

        uint64_t add(T light) {
            if (lights.size() >= max_count) {
                throw std::runtime_error("Lights max count has been reached.");
            }
            lights.emplace_back(std::move(light));
            lights_map.emplace(next_id, lights.size() - 1);
            modified = true;
            return next_id++;
        }

        void remove(uint64_t id) {
            try {
                auto index = lights_map.at(id);

                for (auto &p : lights_map) {
                    if (p.second > index)
                        --p.second;
                }

                lights.erase(lights.begin() + index);
                lights_map.erase(id);
                modified = false;
            } catch (const std::out_of_range& e) {
                throw std::runtime_error("Failed to remove light, no such id.");
            }
        }

        size_t size() const noexcept {
            return lights.size();
        }

        const auto& getLights() const noexcept { return lights; }

        void update() {
            if (modified) {
                light_buffer->mapData(lights.data(), sizeof(T) * lights.size());
                modified = false;
            }
        }
    };

    class DynamicLighting {
    public:
        Light<PointLight> point_lights;
        Light<DirectionalLight> directional_lights;

        DynamicLighting() : point_lights("point_light_buffer"), directional_lights("directional_light_buffer") { }

        void update() {
            point_lights.update();
            directional_lights.update();
        }
    };

    class Lighting {
    public:
        glm::vec4 ambient_color {1.0f, 1.0f, 1.0f, 0.5f};
        DynamicLighting dynamic;

        void update() {
            dynamic.update();
        }
    };
}