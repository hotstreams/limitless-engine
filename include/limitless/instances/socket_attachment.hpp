#pragma once

#include <limitless/models/skeletal_model.hpp>
#include <unordered_map>
#include <string>
#include <glm/glm.hpp>

namespace Limitless {
	class SkeletalInstance;

	template<typename SkeletalInstance = Limitless::SkeletalInstance>
	class SocketAttachment {
		static_assert(std::is_base_of_v<SkeletalInstance, Limitless::SkeletalInstance>, "Template argument should be base of SkeletalInstance!");
	private:
		class Attachment {
		public:
			std::string bone_name;
			glm::mat4 transformation_matrix {1.0f};

			Attachment(std::string _bone_name)
				: bone_name {std::move(_bone_name)} {
			}
		};
		// contains id of InstanceAttachment
		std::unordered_map<uint64_t, Attachment> attachments;
	protected:
		void setTransformation() {
			for (const auto& [id, attachment] : static_cast<SkeletalInstance&>(*this).getAttachments()) {
				if (attachments.find(id) != attachments.end()) {
					attachment->setTransformation(attachments.at(id).transformation_matrix);
				}
			}
		}

		void update() {
			const auto& skeletal = static_cast<const SkeletalModel&>(static_cast<SkeletalInstance&>(*this).getAbstractModel());
			const auto& bone_transforms = static_cast<SkeletalInstance&>(*this).getBoneTransform();
			const auto& bone_map = skeletal.getBoneMap();
			const auto& bones = skeletal.getBones();

			for (auto& [_, attachment] : attachments) {
				auto& [bone_name, transformation_matrix] = attachment;
				const auto bone_index = bone_map.at(attachment.bone_name);
				transformation_matrix = bone_transforms.at(bone_index) * glm::inverse(bones.at(bone_index).offset_matrix);
			}
		}
	public:
		SocketAttachment() = default;
		virtual ~SocketAttachment() = default;

		template<typename Instance, typename... Args>
		auto& attachToBone(std::string bone_name, Args&&... args) {
			auto& attachment = static_cast<SkeletalInstance&>(*this).template attach<Instance>(std::forward<Args>(args)...);
			attachments.emplace(attachment->getId(), std::move(bone_name));
			return attachment;
		}
	};
}
