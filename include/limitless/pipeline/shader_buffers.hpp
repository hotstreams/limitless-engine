#pragma once

namespace Limitless {
    /**
     * Contains buffer names and additional data used in shaders on GPU
     */
    class PipelineShaderBuffers {
    public:
        /**
         * Scene data buffer is used to contain information about current scene
         *
         * check SceneDataStorage class for more info
         */
        static constexpr auto SCENE_DATA_BUFFER_NAME = "scene_data";
    };
}