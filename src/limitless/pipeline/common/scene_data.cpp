#include <limitless/pipeline/common/scene_data.hpp>

#include <limitless/pipeline/shader_buffers.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/core/context.hpp>
#include <limitless/camera.hpp>

using namespace Limitless;

SceneDataStorage::SceneDataStorage(Context& ctx) {
    BufferBuilder builder;
    buffer = builder.setTarget(Buffer::Type::Uniform)
                    .setUsage(Buffer::Usage::DynamicDraw)
                    .setAccess(Buffer::MutableAccess::WriteOrphaning)
                    .setDataSize(sizeof(SceneData))
                    .build(PipelineShaderBuffers::SCENE_DATA_BUFFER_NAME, ctx);
}

SceneDataStorage::~SceneDataStorage() {
    if (auto* ctx = ContextState::getState(glfwGetCurrentContext()); ctx) {
        ctx->getIndexedBuffers().remove(PipelineShaderBuffers::SCENE_DATA_BUFFER_NAME, buffer);
    }
}

void SceneDataStorage::update(Context& context, const Camera& camera) {
    scene_data.projection = camera.getProjection();
    scene_data.projection_inverse = glm::inverse(camera.getProjection());
    scene_data.projection_to_screen = camera.getViewToScreen();
    scene_data.view = camera.getView();
    scene_data.view_inverse = glm::inverse(camera.getView());
    scene_data.VP = camera.getProjection() * camera.getView();
    scene_data.VP_inverse = glm::inverse(scene_data.VP);
    scene_data.camera_position = { camera.getPosition(), 1.0f };
    //TODO: actually context.size not required to be = to framebuffer.size
    scene_data.resolution = context.getSize();
    scene_data.far_plane = camera.getFar();
    scene_data.near_plane = camera.getNear();

    buffer->mapData(&scene_data, sizeof(SceneData));

    buffer->bindBase(context.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::UniformBuffer, PipelineShaderBuffers::SCENE_DATA_BUFFER_NAME));
}

void SceneDataStorage::onFramebufferChange(glm::uvec2 size) {
    scene_data.resolution = size;
}
