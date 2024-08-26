#include <limitless/instances/instanced_instance.hpp>
#include <limitless/core/shader/shader_program.hpp>
#include <limitless/scene.hpp>

using namespace Limitless;

InstancedInstance::InstancedInstance()
    : Instance {InstanceType::Instanced, glm::vec3{0.0f}}
    , buffer {Buffer::builder()
        .target(Buffer::Type::ShaderStorage)
        .usage(Buffer::Usage::DynamicDraw)
        .access(Buffer::MutableAccess::WriteOrphaning)
        .data(nullptr)
        .size(sizeof(glm::mat4) * 1)
        .build("model_buffer", *Context::getCurrentContext())} {
}

InstancedInstance::InstancedInstance(const InstancedInstance& rhs)
    : Instance(rhs)
    , buffer {Buffer::builder()
        .target(Buffer::Type::ShaderStorage)
        .usage(Buffer::Usage::DynamicDraw)
        .access(Buffer::MutableAccess::WriteOrphaning)
        .data(nullptr)
        .size(sizeof(glm::mat4) * 1)
        .build("model_buffer", *Context::getCurrentContext())} {
    for (const auto& instance : rhs.instances) {
        instances.emplace_back((ModelInstance*)instance->clone().release());
    }
}

void InstancedInstance::updateBoundingBox() noexcept {

}

std::unique_ptr<Instance> InstancedInstance::clone() noexcept {
    return std::make_unique<InstancedInstance>(*this);
}

void InstancedInstance::add(const std::shared_ptr<ModelInstance>& instance) {
    instances.emplace_back(instance);
}

void InstancedInstance::remove(uint64_t id){
    std::remove_if(instances.begin(), instances.end(), [&] (auto& i) { return i->getId() == id; });
}

void InstancedInstance::updateInstanceBuffer() {
    std::vector<glm::mat4> new_data;
    new_data.reserve(visible_instances.size());

    for (const auto& instance : visible_instances) {
        if (instance->isHidden()) {
            continue;
        }

        new_data.emplace_back(instance->getFinalMatrix());
    }

    // if update is needed
    if (new_data != current_data) {
        // ensure buffer size
        auto size = sizeof(glm::mat4) * new_data.size();
        if (buffer->getSize() < size) {
            buffer->resize(size);
        }

        buffer->mapData(new_data.data(), size);

        current_data = new_data;
    }
}

void InstancedInstance::update(Context& context, const Camera& camera) {
    if (instances.empty()) {
        return;
    }

    Instance::update(context, camera);

    for (const auto& instance : instances) {
        instance->update(context, camera);
    }

    updateInstanceBuffer();
}

void InstancedInstance::draw(Context& ctx, const Assets& assets, ShaderType pass, ms::Blending blending, const UniformSetter& uniform_setter) {
    if (hidden || instances.empty()) {
        return;
    }

    const_cast<UniformSetter&>(uniform_setter).add([&] (ShaderProgram& shader) {
        shader.setUniform("outline", 0.0f);
    });

    buffer->bindBase(ctx.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, "model_buffer"));

    // iterates over all meshes
    for (auto& [name, mesh] : instances[0]->getMeshes()) {
        mesh.draw_instanced(ctx, assets, pass, shader_type, model_matrix, blending, uniform_setter, visible_instances.size());
    }
}

void InstancedInstance::setVisible(const std::vector<std::shared_ptr<ModelInstance>> &visible) {
    visible_instances = visible;
    //TODO: update buffer?
}
