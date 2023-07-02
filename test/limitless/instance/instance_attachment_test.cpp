#include "../catch_amalgamated.hpp"
#include "../opengl_state.hpp"

#include <limitless/core/context.hpp>
#include <limitless/assets.hpp>

#include <limitless/loaders/model_loader.hpp>
#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/models/abstract_model.hpp>

#include <stdexcept>
#include <limitless/camera.hpp>

using namespace Limitless;
using namespace LimitlessTest;

TEST_CASE("InstanceAttachment tests") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    assets.models.add("backpack", ModelLoader::loadModel(assets, "../assets/models/backpack/backpack.obj", {{
       ::ModelLoaderOption::FlipUV
    }}));

    InstanceAttachment attachment = InstanceAttachment();

    std::unique_ptr<AbstractInstance> instance = std::make_unique<ModelInstance>(assets.models.at("backpack"), glm::vec3{0.0f});

    std::unique_ptr<AbstractInstance> instance1 = std::make_unique<ModelInstance>(assets.models.at("backpack"), glm::vec3{0.0f});

    auto id = instance->getId();
    auto id1 = instance1->getId();

    attachment.attach(std::move(instance));

    REQUIRE_NOTHROW(attachment.getAttachment(id));
    REQUIRE_THROWS_AS(attachment.getAttachment(322), no_such_attachment);
    REQUIRE(!attachment.getAttachments().empty());
    REQUIRE_NOTHROW(attachment.detach(322));

    attachment.detach(id);

    REQUIRE(attachment.getAttachments().empty());


    attachment.attach(std::move(instance1));

    InstanceAttachment copy = attachment;

    REQUIRE_THROWS_AS(copy.getAttachment(id), no_such_attachment);
    REQUIRE(!copy.getAttachments().empty());
    REQUIRE(attachment.getAttachments().begin()->first.id != copy.getAttachments().begin()->first.id);

    check_opengl_state();
}
