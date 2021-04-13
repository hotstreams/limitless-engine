#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace LimitlessEngine {
    class EffectInstance;
    class Lighting;

    class EffectAttachable {
    private:
        struct Attachment {
            std::unique_ptr<EffectInstance> instance;
            glm::vec3 offset;

            Attachment(const glm::vec3& offset, const std::shared_ptr<EffectInstance>& effect, const glm::vec3& position, const glm::vec3& rotation = glm::vec3{0.f}) noexcept;
            Attachment(const glm::vec3& offset, Lighting* lighting, const std::shared_ptr<EffectInstance>& effect, const glm::vec3& position, const glm::vec3& rotation = glm::vec3{0.f}) noexcept;

            Attachment(const Attachment&) noexcept;
            Attachment& operator=(const Attachment&) noexcept;

            Attachment(Attachment&&) noexcept;
            Attachment& operator=(Attachment&&) noexcept;
        };
        std::vector<Attachment> attachments;
    protected:
        virtual void update();

        void setPosition(const glm::vec3& position) noexcept;
        void setRotation(const glm::quat& rotation) noexcept;
    public:
        EffectAttachable() noexcept;
        virtual ~EffectAttachable();

        EffectAttachable(const EffectAttachable&) noexcept;
        EffectAttachable& operator=(const EffectAttachable&) noexcept;

        EffectAttachable(EffectAttachable&&) noexcept;
        EffectAttachable& operator=(EffectAttachable&&) noexcept;

        template<typename... Args>
        EffectInstance& attachEffect(const glm::vec3& offset, Args&&... args) {
            return *attachments.emplace_back(offset, std::forward<Args>(args)...).instance;
        }

        void detachEffect(uint64_t index) noexcept;

        Attachment& operator[](uint64_t index) noexcept;
        Attachment& at(uint64_t index);

        auto begin() noexcept { return attachments.begin(); }
        auto end() noexcept { return attachments.end(); }

        [[nodiscard]] auto begin() const noexcept { return attachments.cbegin(); }
        [[nodiscard]] auto end() const noexcept { return attachments.cend(); }
    };
}
