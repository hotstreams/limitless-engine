#include "../catch_amalgamated.hpp"
#include "../opengl_state.hpp"

#include <limitless/core/context.hpp>
#include <limitless/assets.hpp>

#include <limitless/loaders/model_loader.hpp>
#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/models/abstract_model.hpp>

#include <limitless/camera.hpp>

using namespace Limitless;
using namespace LimitlessTest;

TEST_CASE("SocketAttachment tests") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    assets.models.add("boblamp", ModelLoader::loadModel(assets, "../assets/models/boblamp/boblampclean.md5mesh", {{
      ::ModelLoaderOption::FlipUV
    }}));

    SkeletalInstance instance = SkeletalInstance(assets.models.at("boblamp"), glm::vec3{0.0f});
    std::unique_ptr<Instance> instance1 = std::make_unique<SkeletalInstance>(assets.models.at("boblamp"), glm::vec3{0.0f});
    std::unique_ptr<Instance> instance2 = std::make_unique<SkeletalInstance>(assets.models.at("boblamp"), glm::vec3{0.0f});
    std::unique_ptr<Instance> instance3 = std::make_unique<SkeletalInstance>(assets.models.at("boblamp"), glm::vec3{0.0f});

    auto id = instance2->getId();
    auto id3 = instance3->getId();

    REQUIRE_THROWS_AS(instance.attachToBone("shrek bonner", std::move(instance1)), no_such_bone);

    REQUIRE_NOTHROW(instance.attachToBone("upperarm.R", std::move(instance2)));

    REQUIRE_NOTHROW(instance.getAttachment(id));
    REQUIRE_THROWS_AS(instance.getAttachment(322), no_such_attachment);
    REQUIRE(!instance.getAttachmentBoneData().empty());
    REQUIRE(!instance.getAttachments().empty());
    REQUIRE_NOTHROW(instance.detach(322));
    REQUIRE_NOTHROW(instance.detach(id));
    REQUIRE(instance.getAttachments().empty());

    instance.attachToBone("upperarm.R", std::move(instance3));

    auto copy = instance.clone();

    auto skeletal_copy = static_cast<SkeletalInstance&>(*copy); //NOLINT

    REQUIRE_THROWS_AS(skeletal_copy.getAttachment(id3), no_such_attachment);
    REQUIRE(!skeletal_copy.getAttachments().empty());
    REQUIRE(!skeletal_copy.getAttachmentBoneData().empty());

    REQUIRE(skeletal_copy.getAttachments().begin()->first.id != instance.getAttachments().begin()->first.id);
    REQUIRE(skeletal_copy.getAttachmentBoneData().begin()->first != instance.getAttachmentBoneData().begin()->first);

    check_opengl_state();
}
