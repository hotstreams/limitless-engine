#pragma once

namespace GraphicsEngine {
    struct SceneData {
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
        SceneDataStorage() {
            std::vector<SceneData> data(1);
            buffer = BufferBuilder::buildIndexed("scene_data", Buffer::Type::Uniform, data, Buffer::Usage::StreamDraw, Buffer::MutableAccess::WriteOrphaning);
        }

        void update(Context& context, Scene& scene, Camera& camera) {
            scene_data.VP = camera.getProjection() * camera.getView();
            scene_data.camera_position = { camera.getPosition(), 1.0f };
            scene_data.ambient_color = scene.lighting.ambient_color;
            scene_data.point_lights_size = scene.lighting.dynamic.points_lights.size();
            buffer->mapData(&scene_data, sizeof(SceneData));
        }
    };
}