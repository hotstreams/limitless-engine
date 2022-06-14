#pragma once

#include <memory>
#include <glm/glm.hpp>

namespace Limitless {
    class Context;
    class Camera;
    class Buffer;

    struct SceneData {
        glm::mat4 projection {1.0f};
        glm::mat4 projection_inverse {1.0f};
        glm::mat4 view {1.0f};
        glm::mat4 view_inverse {1.0f};
        glm::mat4 VP {1.0f};
        glm::mat4 VP_inverse {1.0f};
        glm::vec4 camera_position {};
        glm::vec2 resolution {};
        float far_plane;
        float near_plane;
    };

    class SceneDataStorage final {
    private:
        SceneData scene_data;
        std::shared_ptr<Buffer> buffer;
    public:
        explicit SceneDataStorage(Context& context);
        ~SceneDataStorage();

        SceneDataStorage(const SceneDataStorage&) = delete;
        SceneDataStorage(SceneDataStorage&&) = delete;

        void update(Context& context, const Camera& camera);

        void onFramebufferChange(glm::uvec2 size);
    };
}
