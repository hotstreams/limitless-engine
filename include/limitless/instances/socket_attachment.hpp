#pragma once

#include <limitless/models/skeletal_model.hpp>
#include <map>
#include <stdexcept>
#include <string>
#include <glm/glm.hpp>

namespace Limitless {
    class no_such_bone : std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };
}

namespace Limitless {
    class Instance;
    /**
     * SocketAttachment is a class that allows you to attach instance to specific bones of SkeletalInstance
     */
	class SocketAttachment {
	private:
        /**
         * Contains additional data about instance attachments
         */
		class Attachment {
		public:
            /**
             * To which bone it is attached to
             */
			std::string bone_name;

            /**
             * Attached bone transformation matrix
             */
			glm::mat4 transformation_matrix {1.0f};

            explicit Attachment(std::string bone_name) noexcept;
		};

        /**
         * Contains attachment data for InstanceAttachment
         *
         * [id, attachment_info]
         */
		std::map<uint64_t, Attachment> attachment_data;
	protected:
        /**
         * Calculates transformation matrix using bone_transforms from SkeletalInstance
         * and specified bone matrix to which instance attached to
         *
         * Sets this transformation matrix to attached instances
         */
        void updateSocketAttachments();
	public:
		SocketAttachment() = default;
		virtual ~SocketAttachment() = default;

        SocketAttachment(const SocketAttachment&) noexcept;
        SocketAttachment(SocketAttachment&&) = default;

        /**
         * Attaches instance to specified bone
         *
         * throws no_such_bone if not found
         */
        void attachToBone(std::string bone_name, std::shared_ptr<Instance> instance);

        /**
         * Detaches instance from bone
         */
        void detachFromBone(uint64_t id);

        const auto& getAttachmentBoneData() const noexcept { return attachment_data; }
	};
}
