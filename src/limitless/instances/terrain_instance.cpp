#include <limitless/instances/terrain_instance.hpp>

using namespace Limitless;

void TerrainInstance::evaluate() {
    if (tessellation_level > ContextInitializer::limits.max_tess_level) {
        const auto instance_count = std::ceil(tessellation_level / ContextInitializer::limits.max_tess_level);
        auto cp = instances.back()->clone();
        const auto new_size = size / instance_count;
        cp->setScale(glm::vec3{new_size, 1.0f, new_size});
        cp->getMeshes().begin()->second.getMaterial()[0].getTesselationFactor().setValue(glm::vec2(ContextInitializer::limits.max_tess_level));

        instances.clear();
        for (int i = 0; i < instance_count; ++i) {
            for (int j = 0; j < instance_count; ++j) {
                auto instance = std::make_unique<ModelInstance>(*cp);
                instance->setPosition({position.x + new_size * i, position.y, position.z + new_size * j});
                addInstance(std::move(instance));
            }
        }
        delete cp;
    } else {
        instances.back()->setScale({size, 1.0, size});
    }
}

TerrainInstance::TerrainInstance(const std::shared_ptr<AbstractModel>& model, std::shared_ptr<ms::Material> terrain_material, const glm::vec3& position, uint32_t _size)
    : InstancedInstance(position)
    , size {_size} {
    addInstance(std::make_unique<ModelInstance>(model, terrain_material, position));
    evaluate();
}

TerrainInstance& TerrainInstance::setTessLevel(uint32_t _tessellation_level) {
    tessellation_level = _tessellation_level;
    evaluate();
    return *this;
}

TerrainInstance* TerrainInstance::clone() noexcept {
    return new TerrainInstance(*this);
}
