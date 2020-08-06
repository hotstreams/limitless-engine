#include <scene_data.hpp>

using namespace GraphicsEngine;

SceneDataStorage::SceneDataStorage() {
    std::vector<SceneData> data(1);
    buffer = BufferBuilder::buildIndexed("scene_data", Buffer::Type::Uniform, data, Buffer::Usage::StreamDraw, Buffer::MutableAccess::WriteOrphaning);
}

void SceneDataStorage::update([[maybe_unused]] Context& context, Scene& scene, Camera& camera) {
    scene_data.projection = camera.getProjection();
    scene_data.view = camera.getView();
    scene_data.VP = camera.getProjection() * camera.getView();
    scene_data.camera_position = { camera.getPosition(), 1.0f };
    scene_data.ambient_color = scene.lighting.ambient_color;
    scene_data.point_lights_size = scene.lighting.dynamic.point_lights.size();
    buffer->mapData(&scene_data, sizeof(SceneData));
}