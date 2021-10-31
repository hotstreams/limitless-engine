#pragma once

#include <memory>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Limitless {
	class AbstractInstance;
	class Context;
	class Camera;

	class InstanceAttachment {
	private:
		std::unordered_map<uint64_t, std::unique_ptr<AbstractInstance>> attachments;
	protected:
		InstanceAttachment& setParent(const glm::mat4& parent) noexcept;
	public:
		InstanceAttachment() = default;
		virtual ~InstanceAttachment() = default;

		InstanceAttachment(const InstanceAttachment&);
		InstanceAttachment(InstanceAttachment&&) = default;

		void updateAttachments(Context& context, const Camera& camera);

		void attach(std::unique_ptr<AbstractInstance> attachment);
		void detach(uint64_t id);

		const std::unique_ptr<AbstractInstance>& getAttachment(uint64_t id) const;
		std::unique_ptr<AbstractInstance>& getAttachment(uint64_t id);

		auto& getAttachments() noexcept { return attachments; }
		const auto& getAttachments() const noexcept { return attachments; }

		template<typename Instance, typename... Args>
		auto& attach(Args&&... args) {
			auto instance = std::make_unique<Instance>(std::forward<Args>(args)...);
			const auto [it, success] = attachments.emplace(instance->getId(), std::move(instance));
			return it->second;
		}
	};
}
