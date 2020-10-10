#pragma once

#include <glm/glm.hpp>
#include <memory>
#include <vector>

namespace GraphicsEngine {
    class EffectInstance;

    class EffectAttachable {
    private:
        struct Attachment {
            std::unique_ptr<EffectInstance> instance;
            glm::vec3 offset;

            Attachment(const EffectInstance& instance, const glm::vec3& offset) noexcept;
            Attachment(EffectInstance&& instance, const glm::vec3& offset) noexcept;

            Attachment(const Attachment&) noexcept;
            Attachment& operator=(const Attachment&) noexcept;

            Attachment(Attachment&&) noexcept;
            Attachment& operator=(Attachment&&) noexcept;
        };
        std::vector<Attachment> attachments;
    protected:
        void update();

        void setPosition(const glm::vec3& position) noexcept;
        void setRotation(const glm::vec3& rotation) noexcept;
    public:
        EffectAttachable() noexcept;
        virtual ~EffectAttachable();

        EffectAttachable(const EffectAttachable&) noexcept;
        EffectAttachable& operator=(const EffectAttachable&) noexcept;

        EffectAttachable(EffectAttachable&&) noexcept;
        EffectAttachable& operator=(EffectAttachable&&) noexcept;

        template<typename T>
        T& attach(T&& effect_instance, const glm::vec3& offset) {
            return *attachments.emplace_back(std::forward<T>(effect_instance), offset).instance;
        }

        void detach(uint64_t index) noexcept;

        Attachment& operator[](uint64_t index) noexcept;
        Attachment& at(uint64_t index);

        auto begin() noexcept { return attachments.begin(); }
        auto end() noexcept { return attachments.end(); }

        [[nodiscard]] auto begin() const noexcept { return attachments.cbegin(); }
        [[nodiscard]] auto end() const noexcept { return attachments.cend(); }
    };
}
