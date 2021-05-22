#pragma once

#include <limitless/instances/model_instance.hpp>
#include <limitless/core/buffer_builder.hpp>
#include <limitless/core/context_state.hpp>

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

        void calculateBoundingBox() noexcept override {
            //TODO
        }

        void updateBuffer(Context& context, Camera& camera) {
            checkSize();

            std::vector<glm::mat4> data;
            data.reserve(instances.size());

            for (const auto& instance : instances) {
                if (instance->isHidden()) {
                    continue;
                }

                instance->update(context, camera);

                data.emplace_back(instance->getModelMatrix());
            }

            buffer->mapData(data.data(), data.size() * sizeof(glm::mat4));
        }

        InstancedInstance(Lighting* lighting, ModelShader shader, const glm::vec3& position, uint32_t count)
                : AbstractInstance(lighting, shader, position) {
            initializeBuffer(count);
        }

        explicit InstancedInstance(ModelShader shader, const glm::vec3& position, uint32_t count)
                : AbstractInstance(nullptr, shader, position) {
            initializeBuffer(count);
        }
    public:
        InstancedInstance(Lighting* lighting, const glm::vec3& position, uint32_t count = 4)
            : AbstractInstance(lighting, ModelShader::Instanced, position) {
            initializeBuffer(count);
        }

        explicit InstancedInstance(const glm::vec3& position, uint32_t count = 4)
            : AbstractInstance(nullptr, ModelShader::Instanced, position) {
            initializeBuffer(count);
        }

        ~InstancedInstance() override = default;

        InstancedInstance* clone() noexcept override {
            //TODO
            return nullptr;
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

        void update(Context& context, Camera& camera) override {
            if (instances.empty()) {
                return;
            }

            AbstractInstance::update(context, camera);

            updateBuffer(context, camera);
        }

        void draw(Context& ctx, const Assets& assets, ShaderPass pass, ms::Blending blending, const UniformSetter& uniform_set) override {
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

//    template<>
//    class InstancedInstance<SkeletalInstance> final : protected InstancedInstance<ModelInstance> {
//    private:
//        std::shared_ptr<Buffer> buffer;
//
//        void initializeBuffer(uint32_t count) {
//            BufferBuilder builder;
//            buffer = builder.setTarget(Buffer::Type::ShaderStorage)
//                    .setUsage(Buffer::Usage::DynamicDraw)
//                    .setAccess(Buffer::MutableAccess::WriteOrphaning)
//                    .setData(nullptr)
//                    .setDataSize(sizeof(glm::mat4) * count)
//                    .build("model_buffer", *ContextState::getState(glfwGetCurrentContext()));
//        }
//
//        void checkSize() override {
//            InstancedInstance<ModelInstance>::checkSize();
//
//            const auto bone_count = static_cast<SkeletalInstance&>(*instances.at(0)).getModel().getBones().size();
//            const auto buffer_required_size = sizeof(uint32_t) + sizeof(glm::mat4) * bone_count * instances.size();
//
//            if (buffer->getSize() < buffer_required_size) {
//                buffer->resize(buffer_required_size);
//            }
//        }
//
//        void calculateBoundingBox() noexcept override {
//            //TODO
//        }
//
//        void updateBuffer(Context& context, Camera& camera) {
//            checkSize();
//
//            std::vector<glm::mat4> data;
//
//            const auto bone_count = static_cast<SkeletalInstance&>(*instances.at(0)).getModel().getBones().size();
//            const auto required_size = sizeof(glm::mat4) * bone_count * instances.size();
//            data.reserve(required_size);
//
//            for (const auto& instance : instances) {
//                if (instance->isHidden()) {
//                    continue;
//                }
//
//                instance->update(context, camera);
//
//                const auto& bones = static_cast<SkeletalInstance&>(*instance).getBoneTransform();
//                data.insert(data.end(), bones.begin(), bones.end());
//            }
//
//            const auto count = instances.size();
//            buffer->bufferSubData(0, sizeof(uint32_t), &count);
//            buffer->bufferSubData(sizeof(uint32_t), sizeof(glm::mat4) * data.size(), data.data());
//        }
//    public:
//        InstancedInstance(Lighting* lighting, const glm::vec3& position, uint32_t count = 4)
//            : InstancedInstance<ModelInstance>(lighting, ModelShader::SkeletalInstanced, position, count) {
//            InstancedInstance<SkeletalInstance>::initializeBuffer(count);
//        }
//
//        explicit InstancedInstance(const glm::vec3& position, uint32_t count = 4)
//            : InstancedInstance<ModelInstance>(nullptr, ModelShader::SkeletalInstanced, position, count) {
//            InstancedInstance<SkeletalInstance>::initializeBuffer(count);
//        }
//
//        ~InstancedInstance() override = default;
//
//        void addInstance(std::unique_ptr<SkeletalInstance> instance) {
//            instances.emplace_back(std::move(instance));
//        }
//
//        void removeInstance(size_t index) {
//            instances.erase(instances.begin() + index);
//        }
//
//        void update(Context& context, Camera& camera) override {
//            if (instances.empty()) {
//                return;
//            }
//
//            InstancedInstance<ModelInstance>::update(context, camera);
//
//            updateBuffer(context, camera);
//        }
//
//        InstancedInstance* clone() noexcept override {
//            return new InstancedInstance(*this);
//        }
//
//        void draw(Context& ctx, const Assets& assets, ShaderPass pass, ms::Blending blending, const UniformSetter& uniform_set) override {
//            if (hidden || instances.empty()) {
//                return;
//            }
//
//            buffer->bindBase(ctx.getIndexedBuffers().getBindingPoint(IndexedBuffer::Type::ShaderStorage, "model_buffer"));
//
//            InstancedInstance<ModelInstance>::draw(ctx, assets, pass, blending, uniform_set);
//        }
//    };
}