#pragma once

#include <memory>
#include <map>
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>
#include <stdexcept>

namespace Limitless {
    class Instance;
    class Context;
    class Camera;

    class no_such_attachment : public std::runtime_error {
    public:
        using runtime_error::runtime_error;
    };
}

namespace Limitless {
    /**
     * InstanceAttachment is base class for any instance that can have an attachments of another instances
     *
     * instances attached to this instance ARE OWNED BY this instance,
     * it means they become not managed if this instance dies
     */
	class InstanceAttachment {
    public:
        /**
         * Contains attachment types
         */
        enum class AttachmentType {
            Basic,
            Bone
        };

        /**
         * Attachment identifier
         */
        struct AttachmentID {
            uint64_t id;
            AttachmentType type;

            bool operator<(const AttachmentID& rhs) const;
        };
	private:
		std::map<AttachmentID, std::shared_ptr<Instance>> attachments;
	protected:
        /**
         * Sets parent matrix to attachments
         */
		InstanceAttachment& setAttachmentsParent(const glm::mat4& parent) noexcept;

        /**
         * Updates attached instances
         */
        void updateAttachments(const Camera& camera);
	public:
		InstanceAttachment() = default;
		virtual ~InstanceAttachment() = default;

        /**
         * Makes a copy, clones all attachments
         */
		InstanceAttachment(const InstanceAttachment&);
		InstanceAttachment(InstanceAttachment&&) = default;

        /**
         * Attaches instance
         */
		void attach(std::shared_ptr<Instance> attachment);

        /**
         * Detaches instance with specified id
         *
         * if there is no such attachment, silently returns
         */
		void detach(uint64_t id);

        /**
         * Returns attachment with specified id
         *
         * throws no_such_attachment if not found
         */
		[[nodiscard]] const Instance& getAttachment(uint64_t id) const;
		Instance& getAttachment(uint64_t id);

		auto& getAttachments() noexcept { return attachments; }
		[[nodiscard]] const auto& getAttachments() const noexcept { return attachments; }
	};
}
