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

TEST_CASE("SkeletalInstance constructor") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    assets.models.add("boblamp", ModelLoader::loadModel(assets, "../assets/models/boblamp/boblampclean.md5mesh", {{
        ::ModelLoaderOption::FlipUV
    }}));

    REQUIRE_NOTHROW(std::make_unique<SkeletalInstance>(assets.models.at("boblamp"), glm::vec3{0.0f}));

    check_opengl_state();
}

TEST_CASE("SkeletalInstance copy constructor") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.load(context);
    Camera camera {{1, 1}};

    assets.models.add("boblamp", ModelLoader::loadModel(assets, "../assets/models/boblamp/boblampclean.md5mesh", {{
      ::ModelLoaderOption::FlipUV
    }}));

    SkeletalInstance instance = SkeletalInstance(assets.models.at("boblamp"), glm::vec3{0.0f});
    instance.setPosition({0.5f, 0.1f, 0.2f})
            .setRotation(glm::vec3{1.0f, 0.5f, 1.0f})
            .setScale(glm::vec3{0.5f});

    instance.update(context, camera);

    SkeletalInstance copy = instance;

    REQUIRE(instance.getId() != copy.getId());
    REQUIRE(instance.getAbstractModel().getName() == copy.getAbstractModel().getName());
    REQUIRE(instance.getFinalMatrix() == copy.getFinalMatrix());
    REQUIRE(instance.getModelMatrix() == copy.getModelMatrix());
    REQUIRE(instance.getTransformationMatrix() == copy.getTransformationMatrix());
    REQUIRE(instance.getShaderType() == copy.getShaderType());
    REQUIRE(instance.getPosition() == copy.getPosition());
    REQUIRE(instance.getRotation() == copy.getRotation());
    REQUIRE(instance.getScale() == copy.getScale());
    REQUIRE(instance.doesCastShadow() == copy.doesCastShadow());
    REQUIRE(instance.isOutlined() == copy.isOutlined());
    REQUIRE(instance.isHidden() == copy.isHidden());
    REQUIRE(instance.isKilled() == copy.isKilled());

    check_opengl_state();
}

TEST_CASE("SkeletalInstance clone") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};
    assets.load(context);
    Camera camera {{1, 1}};

    assets.models.add("boblamp", ModelLoader::loadModel(assets, "../assets/models/boblamp/boblampclean.md5mesh", {{
                                                                                                                          ::ModelLoaderOption::FlipUV
    }}));

    SkeletalInstance instance = SkeletalInstance(assets.models.at("boblamp"), glm::vec3{0.0f});
    instance.setPosition({0.5f, 0.1f, 0.2f})
            .setRotation(glm::vec3{1.0f, 0.5f, 1.0f})
            .setScale(glm::vec3{0.5f});

    instance.update(context, camera);

    std::unique_ptr<AbstractInstance> copy = instance.clone();

    REQUIRE(instance.getId() != copy->getId());
    REQUIRE(instance.getFinalMatrix() == copy->getFinalMatrix());
    REQUIRE(instance.getModelMatrix() == copy->getModelMatrix());
    REQUIRE(instance.getTransformationMatrix() == copy->getTransformationMatrix());
    REQUIRE(instance.getShaderType() == copy->getShaderType());
    REQUIRE(instance.getPosition() == copy->getPosition());
    REQUIRE(instance.getRotation() == copy->getRotation());
    REQUIRE(instance.getScale() == copy->getScale());
    REQUIRE(instance.doesCastShadow() == copy->doesCastShadow());
    REQUIRE(instance.isOutlined() == copy->isOutlined());
    REQUIRE(instance.isHidden() == copy->isHidden());
    REQUIRE(instance.isKilled() == copy->isKilled());

    check_opengl_state();
}

TEST_CASE("SkeletalInstance animation") {
    Context context = {"Title", {1, 1}, {{WindowHint::Visible, false}}};
    Assets assets {"../assets"};

    assets.models.add("boblamp", ModelLoader::loadModel(assets, "../assets/models/boblamp/boblampclean.md5mesh", {{
      ::ModelLoaderOption::FlipUV
    }}));

    SkeletalInstance instance = SkeletalInstance(assets.models.at("boblamp"), glm::vec3{0.0f});

    instance.play("");

    REQUIRE(instance.getCurrentAnimation() != nullptr);
    REQUIRE(instance.isPaused() == false);
    REQUIRE(instance.getAllAnimations().size() != 0);
    REQUIRE(instance.getBoneTransform().size() != 0);

    instance.pause();

    REQUIRE(instance.isPaused() == true);

    check_opengl_state();
}
