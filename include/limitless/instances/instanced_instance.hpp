#pragma once

#include <limitless/instances/model_instance.hpp>
#include <limitless/core/buffer/buffer_builder.hpp>
#include <limitless/core/context.hpp>

namespace Limitless {
    template<typename Instance, typename = void>
    class InstancedInstance : public AbstractInstance {
        static_assert(std::is_same_v<Instance, Instance>, "InstancedInstance for this typename is unimplemented!");
    };

    template<>
    class InstancedInstance<ModelInstance> : public AbstractInstance {
    protected:
        // contains instanced models
        std::vector<std::unique_ptr<ModelInstance>> instances;

        // contains model matrices for each ModelInstance
        std::shared_ptr<Buffer> buffer;

        void initializeBuffer(uint32_t count) {
            BufferBuilder builder;
            buffer = builder.setTarget(Buffer::Type::ShaderStorage)
                    .setUsage(Buffer::Usage::DynamicDraw)
                    .setAccess(Buffer::MutableAccess::WriteOrphaning)
                    .setData(nullptr)
                    .setDataSize(sizeof(glm::mat4) * count)
                    .build("model_buffer", *ContextState::getState(glfwGetCurrentContext()));
        }

        virtual void checkSize() {
            if (buffer->getSize() < sizeof(glm::mat4) * instances.size()) {
                buffer->resize(sizeof(glm::mat4) * instances.size());
            }
        }

        void updateBoundingBox() noexcept override {
            assert("RIP");
        }

        void updateBuffer(Context& context, const Camera& camera) {
            checkSize();

            std::vector<glm::mat4> data;
            data.reserve(instances.size());

            for (const auto& instance : instances) {
                //TODO
//                if (instance->isHidden()) {
//                    continue;
//                }

                instance->update(context, camera);

                data.emplace_back(instance->getModelMatrix());
            }

            buffer->mapData(data.data(), data.size() * sizeof(glm::mat4));
        }

        explicit InstancedInstance(InstanceType shader, const glm::vec3& position, uint32_t count)
            : AbstractInstance(shader, position) {
            initializeBuffer(count);
        }
    public:
        explicit InstancedInstance(const glm::vec3& position, uint32_t count = 4)
            : AbstractInstance(InstanceType::Instanced, position) {
            initializeBuffer(count);
        }

        ~InstancedInstance() override = default;

        InstancedInstance(const InstancedInstance& rhs)
            : AbstractInstance(rhs.shader_type, rhs.position) {
            initializeBuffer(rhs.instances.size());
            for (const auto& instance : rhs.instances) {
                instances.emplace_back((ModelInstance*)instance->clone().release());
            }
        }
        InstancedInstance(InstancedInstance&&) noexcept = default;

        std::unique_ptr<AbstractInstance> clone() noexcept override {
            return std::make_unique<InstancedInstance>(*this);
        }

        void addInstance(std::unique_ptr<ModelInstance> instance) {
            instances.emplace_back(std::move(instance));
        }

        void removeInstance(size_t index) {
            instances.erase(instances.begin() + index);
        }

        auto& getInstances() noexcept { return instances; }
        [[nodiscard]] const auto& getInstances() const noexcept { return instances; }

        ModelInstance& operator[](size_t index) { return *instances[index]; }
        const ModelInstance& operator[](size_t index) const { return *instances[index]; }

        ModelInstance& at(size_t index) { return *instances.at(index); }
        [[nodiscard]] const ModelInstance& at(size_t index) const { return *instances.at(index); }

        void update(Context& context, const Camera& camera) override {
            if (instances.empty()) {
                return;
            }

            AbstractInstance::update(context, camera);

            updateBuffer(context, camera);
        }

        void draw(Context& ctx, const Assets& assets, ShaderType pass, ms::Blending blending, const UniformSetter& uniform_set) override {
            if (hidden || instances.empty()) {
                return;
            }

            buffer->bindBase(ctx.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, "model_buffer"));

            // iterates over all meshes
            for (auto& [name, mesh] : instances[0]->getMeshes()) {
                mesh.draw_instanced(ctx, assets, pass, shader_type, model_matrix, blending, uniform_set, instances.size());
            }
        }
    };
}