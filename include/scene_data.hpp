#pragma once

#include <buffer.hpp>
#include <scene.hpp>
#include <camera.hpp>

namespace GraphicsEngine {
    struct SceneData {
        glm::mat4 projection {1.0f};
        glm::mat4 view {1.0f};
        glm::mat4 VP {1.0f};
        glm::vec4 camera_position {0.0f};
        glm::vec4 ambient_color {1.0f};
        uint32_t point_lights_size {0};
    };

    class SceneDataStorage {
    private:
        SceneData scene_data;
        std::shared_ptr<Buffer> buffer;
    public:
        SceneDataStorage();

        void update(Context& context, Scene& scene, Camera& camera);
    };
}