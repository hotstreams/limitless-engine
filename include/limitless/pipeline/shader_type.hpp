#pragma once

#include <limitless/util/filesystem.hpp>
#include <set>
#include <map>

namespace Limitless {
    /**
     * ShaderType and InstanceType are used to determine which shader to use to render objects
     *
     * ShaderType determines which shader should be used for chosen pipeline/renderer
     *
     * InstanceType determines which shader should be used for chosen instance type
     *
     * and only with both [shader_type, instance_type] we can pick a particular shader to render whatever we want
     */

    /**
     * ShaderPass defines set of different shaders which are used for different purposes
     *
     * Each ShaderPass depends on object's used material, so it is complied against each material type
     */
    enum class ShaderType {
        /**
         *  Forward shader just renders object as it is
         *
         *  used in Forward  Pipeline
         *  used in Deferred Pipeline to render transparent objects
         *
         *  note: you can use this shader to render anything just on-top of you framebuffer
         */
        Forward,

        /**
         *  Depth shader describes shader that renders only to depth buffer,
         *  no color/lighting computations are made if it is not discarding fragments
         *
         *  used in Deferred Pipeline to fill GBUFFER depth texture
         */
        Depth,

        /**
         * GBuffer shader describes shader that renders material properties to buffer
         *
         * used in Deferred Pipeline to fill GBUFFER properties
         */
        GBuffer,

        /**
         * Skybox shader
         */
        Skybox,

        /**
         * DirectionalShadow shader implements cascade shadow maps for directional light
         */
        DirectionalShadow,

        /**
         * ColorPicker shader allows you to get pixel values back from rendered objects for picking objects
         */
        ColorPicker
    };

    inline const std::map<ShaderType, std::string> SHADER_PASS_PATH = {
        {ShaderType::Forward,           "pipeline"  PATH_SEPARATOR "forward" PATH_SEPARATOR "forward" },

        {ShaderType::Depth,             "pipeline"  PATH_SEPARATOR "deferred" PATH_SEPARATOR "depth" },
        {ShaderType::GBuffer,           "pipeline"  PATH_SEPARATOR "deferred" PATH_SEPARATOR "gbuffer" },
        {ShaderType::Skybox,            "pipeline"  PATH_SEPARATOR "deferred" PATH_SEPARATOR "skybox"},

        {ShaderType::DirectionalShadow, "pipeline"  PATH_SEPARATOR "lighting" PATH_SEPARATOR "directional_shadows" },

        {ShaderType::ColorPicker,       "pipeline"  PATH_SEPARATOR "util"     PATH_SEPARATOR "color_picker" },
    };

    /**
     * InstanceType defines shader type to render different types of instances
     *
     * Each InstanceType depends on object's used instance type, so it is complied against each instance type
     */
    enum class InstanceType {
        Model,
        Skeletal,
        Instanced,
        SkeletalInstanced,
        Effect
    };

    using ShaderTypes = std::set<ShaderType>;
    using InstanceTypes = std::set<InstanceType>;

    /**
     * ShaderKey describes unique shader used in rendering pipeline
     *
     * We are not using uber shaders, so we have to compile each ShaderType against each model and material types
     */
    class ShaderKey {
    public:
        ShaderType material_type;
        InstanceType model_type;
        uint64_t material_index;

        bool operator<(const ShaderKey& rhs) const noexcept;
    };
}
