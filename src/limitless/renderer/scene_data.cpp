#include <limitless/renderer/scene_data.hpp>

#include <limitless/renderer/shader_buffers.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/core/context.hpp>
#include <limitless/camera.hpp>
#include <limitless/renderer/renderer.hpp>

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

void SceneDataStorage::update(const Camera& camera) {
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

    buffer->mapData(&scene_data, sizeof(SceneData));

    buffer->bindBase(Context::getCurrentContext()->getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::UniformBuffer, PipelineShaderBuffers::SCENE_DATA_BUFFER_NAME));
}

void SceneDataStorage::onFramebufferChange(glm::uvec2 size) {
    scene_data.resolution = size;
}
