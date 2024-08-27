#pragma once

#include <memory>
#include <glm/glm.hpp>

namespace Limitless {
    class Context;
    class Camera;
    class Buffer;
    class Renderer;

    /**
     * SceneData is a mapping structure for GPU
     *
     * contains information about camera, context, current frame
     *
     * used by all shaders to compute MVP matrices, camera effects, and screen space post processing
     */
    class SceneData {
    public:
        glm::mat4 projection {1.0f};
        glm::mat4 projection_inverse {1.0f};
        glm::mat4 projection_to_screen {1.0f};
        glm::mat4 view {1.0f};
        glm::mat4 view_inverse {1.0f};
        glm::mat4 VP {1.0f};
        glm::mat4 VP_inverse {1.0f};
        glm::vec4 camera_position {};
        glm::vec2 resolution {};
        float far_plane {1000.0f};
        float near_plane {0.01f};
    };

    /**
     * SceneDataStorage is a class that responsible for mapping SceneData info to GPU
     */
    class SceneDataStorage final {
    private:
        /**
         * Scene data
         */
        SceneData scene_data;

        /**
         * Corresponding buffer
         */
        std::shared_ptr<Buffer> buffer;
    public:
        SceneDataStorage(Renderer& renderer);
        ~SceneDataStorage();

        SceneDataStorage(const SceneDataStorage&) = delete;
        SceneDataStorage(SceneDataStorage&&) = delete;

        /**
         * Updates data in structure cache
         * Maps data to GPU
         * Binds buffer to GPU
         */
        void update(const Camera& camera);

        /**
         * Updates framebuffer size
         *
         * note: it is not required to Context::size to be equal to Framebuffer::size
         */
        void onFramebufferChange(glm::uvec2 size);
    };
}
