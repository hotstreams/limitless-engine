#include <limitless/pipeline/scene_data.hpp>

#include <limitless/core/context.hpp>
#include <limitless/core/buffer_builder.hpp>
#include <limitless/camera.hpp>

using namespace Limitless;

namespace {
    constexpr auto SCENE_DATA_BUFFER_NAME = "scene_data";
}

SceneDataStorage::SceneDataStorage(Context& ctx) {
    BufferBuilder builder;
    buffer = builder.setTarget(Buffer::Type::Uniform)
                    .setUsage(Buffer::Usage::DynamicDraw)
                    .setAccess(Buffer::MutableAccess::WriteOrphaning)
                    .setDataSize(sizeof(SceneData))
                    .build(SCENE_DATA_BUFFER_NAME, ctx);
}

void SceneDataStorage::update(Context& context, const Camera& camera) {
    scene_data.projection = camera.getProjection();
    scene_data.view = camera.getView();
    scene_data.VP = camera.getProjection() * camera.getView();
    scene_data.camera_position = { camera.getPosition(), 1.0f };
    buffer->mapData(&scene_data, sizeof(SceneData));

    buffer->bindBase(context.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::UniformBuffer, SCENE_DATA_BUFFER_NAME));
}