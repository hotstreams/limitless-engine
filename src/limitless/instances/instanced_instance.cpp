#include <limitless/instances/instanced_instance.hpp>
#include <limitless/instances/model_instance.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/models/model.hpp>
#include <limitless/renderer/renderer_settings.hpp>
#include <limitless/scene.hpp>
#include <limitless/core/cpu_profiler.hpp>
#include <limitless/util/lod_transition.h>

using namespace Limitless;

InstancedInstance::InstancedInstance(InstanceType container_type)
    : Instance {container_type, glm::vec3{0.0f}} {
}

InstancedInstance::InstancedInstance(const InstancedInstance& rhs)
    : Instance(rhs) {
    for (const auto& instance : rhs.instances) {
        instances.emplace_back((ModelInstance*)instance->clone().release());
    }
}

std::unique_ptr<Instance> InstancedInstance::clone() noexcept {
    return std::make_unique<InstancedInstance>(*this);
}

void InstancedInstance::add(const std::shared_ptr<ModelInstance>& instance) {
    instances.emplace_back(instance);
}

void InstancedInstance::remove(uint64_t id){
    auto it = std::remove_if(instances.begin(), instances.end(), [&] (auto& i) { return i->getId() == id; });
    instances.erase(it, instances.end());
}

std::shared_ptr<Buffer>& InstancedInstance::ensureLodBuffer(uint32_t lod) {
    auto it = lod_buffers.find(lod);
    if (it == lod_buffers.end()) {
        auto buffer = Buffer::builder()
            .target(Buffer::Type::ShaderStorage)
            .usage(Buffer::Usage::DynamicDraw)
            .access(Buffer::MutableAccess::WriteOrphaning)
            .data(nullptr)
            .size(sizeof(Data))
            .build();
        it = lod_buffers.emplace(lod, std::move(buffer)).first;
    }
    return it->second;
}

std::shared_ptr<Buffer> InstancedInstance::getLodBuffer(uint32_t lod) const noexcept {
    auto it = lod_buffers.find(lod);
    if (it != lod_buffers.end()) {
        return it->second;
    }
    return nullptr;
}

void InstancedInstance::updateInstanceBuffer() {
    // Group visible instances by LOD level (duplicate into two buckets during dither cross-fade)
    lod_visible_instances.clear();
    for (const auto& instance : visible_instances) {
        const auto& model = *instance->getModel();
        const auto& lg = instance->getLodGroup();

        if (model.getLods().size() > 1u && model.getTransition() == LodTransition::CrossFadeDither && lg.isLodCrossFadeActive()) {
            lod_visible_instances[lg.getCrossFadeFinerLod()].push_back(instance);
            lod_visible_instances[lg.getCrossFadeCoarserLod()].push_back(instance);
        } else {
            lod_visible_instances[lg.getCurrentLod()].push_back(instance);
        }
    }

    // Update per-LOD buffers
    for (auto& [lod, lod_instances] : lod_visible_instances) {
        std::vector<Data> new_data;
        new_data.reserve(lod_instances.size());

        for (const auto& instance : lod_instances) {
            Data row = instance->getCurrentData();
            row.lod_fade = instance->getLodGroup().getLodFadePackedForDrawLod(lod);
            new_data.push_back(row);
        }

        auto& current_data = lod_current_data[lod];

        // Only update if data changed
        if (new_data != current_data) {
            if (!RendererSettings::global_model_instance_ssbo_active) {
                auto& buffer = ensureLodBuffer(lod);
                const auto size = sizeof(Data) * new_data.size();

                if (buffer->getSize() < size) {
                    buffer->resize(size);
                }

                buffer->mapData(new_data.data(), size);
            }
            current_data = std::move(new_data);
        }
    }

    // Clean up stale LOD entries that have no visible instances
    for (auto it = lod_current_data.begin(); it != lod_current_data.end(); ) {
        if (lod_visible_instances.find(it->first) == lod_visible_instances.end()) {
            it = lod_current_data.erase(it);
        } else {
            ++it;
        }
    }
}

void InstancedInstance::update(const Camera &camera) {
    CpuProfileScope scope(global_profiler, "InstancedInstance::update");
    if (instances.empty()) {
        return;
    }

    Instance::update(camera);

    for (const auto& instance : instances) {
        instance->update(camera);
    }

    updateInstanceBuffer();
}

void InstancedInstance::setVisible(const std::vector<std::shared_ptr<ModelInstance>> &visible) {
    visible_instances = visible;
    updateInstanceBuffer();
}
