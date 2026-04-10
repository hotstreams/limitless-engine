#include <limitless/renderer/scene_data.hpp>

#include <limitless/renderer/shader_buffers.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/core/context.hpp>
#include <limitless/camera.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/scene.hpp>

using namespace Limitless;

SceneDataStorage::SceneDataStorage(Renderer& renderer) {
    buffer = Buffer::builder()
            .target(Buffer::Type::Uniform)
            .usage(Buffer::Usage::DynamicDraw)
            .access(Buffer::MutableAccess::WriteOrphaning)
            .size(sizeof(SceneData))
            .build(PipelineShaderBuffers::SCENE_DATA_BUFFER_NAME, *Context::getCurrentContext());

    scene_data.resolution = renderer.getResolution();
}

SceneDataStorage::~SceneDataStorage() {
    if (auto* ctx = Context::getCurrentContext(); ctx) {
        ctx->getIndexedBuffers().remove(PipelineShaderBuffers::SCENE_DATA_BUFFER_NAME, buffer);
    }
}

void SceneDataStorage::update(const Scene& scene, const Camera& camera) {
    scene_data.projection = camera.getProjection();
    scene_data.projection_inverse = glm::inverse(camera.getProjection());
    scene_data.projection_to_screen = camera.getViewToScreen();
    scene_data.view = camera.getView();
    scene_data.view_inverse = glm::inverse(camera.getView());
    scene_data.VP = camera.getProjection() * camera.getView();
    scene_data.VP_inverse = glm::inverse(scene_data.VP);
    scene_data.camera_position = { camera.getPosition(), 1.0f };

    scene_data.far_plane = camera.getFar();
    scene_data.near_plane = camera.getNear();

    // wind
    const auto& wind = scene.getWind();
    if (wind.enabled && wind.strength > 0.0f && wind.quality > 0) {
        scene_data.wind_dir_strength = glm::vec4(glm::normalize(wind.direction), wind.strength);
        scene_data.wind_params = glm::vec4(wind.time, wind.speed, wind.scale, static_cast<float>(wind.quality));
    } else {
        scene_data.wind_dir_strength = glm::vec4(0.0f);
        scene_data.wind_params = glm::vec4(0.0f);
    }

    // SpeedTree wind preset
    if (scene.isSpeedTreeWindCpuStateEnabled()) {
        auto& st_state = const_cast<Scene&>(scene).getSpeedTreeWindState();
        st_state.setStrength(wind.strength);
        st_state.setDirection(wind.direction);
        st_state.advance(wind.enabled && wind.quality > 0 && wind.strength > 0.0f, wind.time);
        const auto& u = st_state.getUniforms();

        scene_data.st_wind_vector = u.wind_vector;
        scene_data.st_wind_global = u.wind_global;
        scene_data.st_wind_branch = u.wind_branch;
        scene_data.st_wind_branch_twitch = u.wind_branch_twitch;
        scene_data.st_wind_branch_whip = u.wind_branch_whip;
        scene_data.st_wind_branch_anchor = u.wind_branch_anchor;
        scene_data.st_wind_branch_adherences = u.wind_branch_adherences;
        scene_data.st_wind_turbulences = u.wind_turbulences;
        scene_data.st_wind_leaf1_ripple = u.wind_leaf1_ripple;
        scene_data.st_wind_leaf1_tumble = u.wind_leaf1_tumble;
        scene_data.st_wind_leaf1_twitch = u.wind_leaf1_twitch;
        scene_data.st_wind_leaf2_ripple = u.wind_leaf2_ripple;
        scene_data.st_wind_leaf2_tumble = u.wind_leaf2_tumble;
        scene_data.st_wind_leaf2_twitch = u.wind_leaf2_twitch;
        scene_data.st_wind_frond_ripple = u.wind_frond_ripple;
        scene_data.st_wind_rolling_branch = u.wind_rolling_branch;
        scene_data.st_wind_rolling_leaf_and_direction = u.wind_rolling_leaf_and_direction;
        scene_data.st_wind_rolling_noise = u.wind_rolling_noise;
        scene_data.st_wind_animation = u.wind_animation;

        // Reuse legacy debug settings (mask + branch mode) until the state has its own.
        const auto& st = scene.getSpeedTreeWind();
        scene_data.st_wind_debug = glm::uvec4(st.debug_mask, 0u, 0u, 0u);
    } else {
        const auto& st = scene.getSpeedTreeWind();
        scene_data.st_wind_vector = st.wind_vector;
        scene_data.st_wind_global = st.wind_global;
        scene_data.st_wind_branch = st.wind_branch;
        scene_data.st_wind_branch_twitch = st.wind_branch_twitch;
        scene_data.st_wind_branch_whip = st.wind_branch_whip;
        scene_data.st_wind_branch_anchor = st.wind_branch_anchor;
        scene_data.st_wind_branch_adherences = st.wind_branch_adherences;
        scene_data.st_wind_turbulences = st.wind_turbulences;
        scene_data.st_wind_leaf1_ripple = st.wind_leaf1_ripple;
        scene_data.st_wind_leaf1_tumble = st.wind_leaf1_tumble;
        scene_data.st_wind_leaf1_twitch = st.wind_leaf1_twitch;
        scene_data.st_wind_leaf2_ripple = st.wind_leaf2_ripple;
        scene_data.st_wind_leaf2_tumble = st.wind_leaf2_tumble;
        scene_data.st_wind_leaf2_twitch = st.wind_leaf2_twitch;
        scene_data.st_wind_frond_ripple = st.wind_frond_ripple;
        scene_data.st_wind_rolling_branch = st.wind_rolling_branch;
        scene_data.st_wind_rolling_leaf_and_direction = st.wind_rolling_leaf_and_direction;
        scene_data.st_wind_rolling_noise = st.wind_rolling_noise;
        scene_data.st_wind_animation = st.wind_animation;
        scene_data.st_wind_debug = glm::uvec4(st.debug_mask, 0u, 0u, 0u);
    }

    buffer->mapData(&scene_data, sizeof(SceneData));

    buffer->bindBase(Context::getCurrentContext()->getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::UniformBuffer, PipelineShaderBuffers::SCENE_DATA_BUFFER_NAME));
}

void SceneDataStorage::onFramebufferChange(glm::uvec2 size) {
    scene_data.resolution = size;
}
