#include <limitless/scene_data.hpp>

#include <limitless/core/context.hpp>
#include <limitless/core/buffer_builder.hpp>
#include <limitless/camera.hpp>
#include <limitless/scene.hpp>

using namespace LimitlessEngine;

SceneDataStorage::SceneDataStorage(Context& context) {
    BufferBuilder builder;
    buffer = builder.setTarget(Buffer::Type::Uniform)
                    .setUsage(Buffer::Usage::DynamicDraw)
                    .setAccess(Buffer::MutableAccess::WriteOrphaning)
                    .setDataSize(sizeof(SceneData))
                    .build("scene_data", context);
}

void SceneDataStorage::update([[maybe_unused]] Context& context, Camera& camera) {
    scene_data.projection = camera.getProjection();
    scene_data.view = camera.getView();
    scene_data.VP = camera.getProjection() * camera.getView();
    scene_data.camera_position = { camera.getPosition(), 1.0f };
    buffer->mapData(&scene_data, sizeof(SceneData));
}