#pragma once

#include <memory>
#include <glm/glm.hpp>

namespace LimitlessEngine {
    class Context;
    class Camera;
    class Buffer;
    class Scene;

    struct SceneData {
        glm::mat4 projection {1.0f};
        glm::mat4 view {1.0f};
        glm::mat4 VP {1.0f};
        glm::vec4 camera_position {};
    };

    class SceneDataStorage {
    private:
        SceneData scene_data;
        std::shared_ptr<Buffer> buffer;
    public:
        SceneDataStorage();

        void update(Context& context, Camera& camera);
    };
}