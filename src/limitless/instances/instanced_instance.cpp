#include <limitless/instances/instanced_instance.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/scene.hpp>
#include <limitless/core/cpu_profiler.hpp>

using namespace Limitless;

InstancedInstance::InstancedInstance()
    : Instance {InstanceType::Instanced, glm::vec3{0.0f}}
    , buffer {Buffer::builder()
        .target(Buffer::Type::ShaderStorage)
        .usage(Buffer::Usage::DynamicDraw)
        .access(Buffer::MutableAccess::WriteOrphaning)
        .data(nullptr)
        .size(sizeof(Data))
        .build("model_buffer", *Context::getCurrentContext())} {
}

InstancedInstance::InstancedInstance(const InstancedInstance& rhs)
    : Instance(rhs)
    , buffer {Buffer::builder()
        .target(Buffer::Type::ShaderStorage)
        .usage(Buffer::Usage::DynamicDraw)
        .access(Buffer::MutableAccess::WriteOrphaning)
        .data(nullptr)
        .size(sizeof(Data))
        .build("model_buffer", *Context::getCurrentContext())} {
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

void InstancedInstance::updateInstanceBuffer() {
    std::vector<Data> new_data;
    new_data.reserve(visible_instances.size());

    for (const auto& instance : visible_instances) {
        new_data.emplace_back(instance->getCurrentData());
    }

    // if update is needed
    if (new_data != current_instance_data) {
        // ensure buffer size
        auto size = sizeof(Data) * new_data.size();
        if (buffer->getSize() < size) {
            buffer->resize(size);
        }

        buffer->mapData(new_data.data(), size);

        current_instance_data = new_data;
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
