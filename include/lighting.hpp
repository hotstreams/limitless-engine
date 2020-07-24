#pragma once

#include <glm/glm.hpp>

namespace GraphicsEngine {
    struct PointLight {
        glm::vec4 position;
        glm::vec4 color;
        float constant;
        float linear;
        float quadratic;
        float radius;
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
        Light<PointLight> points_lights;

        DynamicLighting() : points_lights("point_light_buffer") { }

        void update() {
            points_lights.update();
        }
    };

    class Lighting {
    public:
        DynamicLighting dynamic;
        glm::vec4 ambient_color {1.0f, 1.0f, 1.0f, 0.7f};

        void update() {
            dynamic.update();
        }
    };
}