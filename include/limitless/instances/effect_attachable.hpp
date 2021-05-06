#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace Limitless {
    class EffectInstance;
    class Lighting;
    class Context;
    class Camera;

    class EffectAttachable {
    private:
        struct Attachment {
            std::unique_ptr<EffectInstance> instance;
            glm::vec3 offset;

            Attachment(const glm::vec3& offset, const std::shared_ptr<EffectInstance>& effect, const glm::vec3& position, const glm::vec3& rotation = glm::vec3{0.f}) noexcept;
            Attachment(const glm::vec3& offset, Lighting* lighting, const std::shared_ptr<EffectInstance>& effect, const glm::vec3& position, const glm::vec3& rotation = glm::vec3{0.f}) noexcept;

            Attachment(const Attachment&);
            Attachment(Attachment&&) noexcept = default;
            Attachment& operator=(Attachment&&) = default;
        };
        std::vector<Attachment> attachments;
    protected:
        virtual void update(Context& context, Camera& camera);

        void setPosition(const glm::vec3& position) noexcept;
        void setRotation(const glm::quat& rotation) noexcept;
        void rotateBy(const glm::quat& rotation) noexcept;

        EffectAttachable() noexcept;
        virtual ~EffectAttachable();

        EffectAttachable(const EffectAttachable&);
        EffectAttachable(EffectAttachable&&) noexcept;
    public:
        template<typename... Args>
        EffectInstance& attachEffect(const glm::vec3& offset, Args&&... args) {
            return *attachments.emplace_back(offset, std::forward<Args>(args)...).instance;
        }

        void detachEffect(uint64_t index) noexcept;

        Attachment& operator[](uint64_t index) noexcept;
        Attachment& at(uint64_t index);

        auto& getAttachedEffects() noexcept { return attachments; }
        [[nodiscard]] const auto& getAttachedEffects() const noexcept { return attachments; }
    };
}
