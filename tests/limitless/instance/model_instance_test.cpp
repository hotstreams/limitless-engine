#include "../catch_amalgamated.hpp"
#include "../opengl_state.hpp"

#include <limitless/core/context.hpp>
#include <limitless/assets.hpp>

#include <limitless/loaders/model_loader.hpp>
#include <limitless/instances/model_instance.hpp>
#include <limitless/models/abstract_model.hpp>

#include <stdexcept>
#include <limitless/camera.hpp>

using namespace Limitless;
using namespace LimitlessTest;

TEST_CASE("ModelInstance model constructor") {
    Context context = {"Title", {1, 1}, nullptr, {{WindowHint::Hint::Visible, false}}};
    Assets assets {"../assets"};

    assets.models.add("backpack", ModelLoader::loadModel(assets, "../assets/models/backpack/backpack.obj", {{
       ::ModelLoaderOption::FlipUV
    }}));

    std::unique_ptr<ModelInstance> instance;

    REQUIRE_NOTHROW(std::make_unique<ModelInstance>(assets.models.at("backpack")));

    check_opengl_state();
}

TEST_CASE("ModelInstance model constructor with element model") {
    Context context = {"Title", {1, 1}, nullptr, {{WindowHint::Hint::Visible, false}}};
    Assets assets {"../assets"};
    assets.load(context);

    std::unique_ptr<ModelInstance> instance;

    REQUIRE_THROWS_AS(std::make_unique<ModelInstance>(assets.models.at("cube")), std::logic_error);

    check_opengl_state();
}

TEST_CASE("ModelInstance elementary model constructor") {
    Context context = {"Title", {1, 1}, nullptr, {{WindowHint::Hint::Visible, false}}};
    Assets assets {"../assets"};
    assets.load(context);

    std::unique_ptr<ModelInstance> instance;

    REQUIRE_NOTHROW(std::make_unique<ModelInstance>(assets.models.at("cube"), assets.materials.at("red")));

    check_opengl_state();
}

TEST_CASE("ModelInstance elementary model constructor with model") {
    Context context = {"Title", {1, 1}, nullptr, {{WindowHint::Hint::Visible, false}}};
    Assets assets {"../assets"};
    assets.load(context);

    assets.models.add("backpack", ModelLoader::loadModel(assets, "../assets/models/backpack/backpack.obj", {{
            ::ModelLoaderOption::FlipUV
    }}));

    std::unique_ptr<ModelInstance> instance;

    REQUIRE_THROWS_AS(std::make_unique<ModelInstance>(assets.models.at("backpack"), assets.materials.at("red")), std::logic_error);

    check_opengl_state();
}

TEST_CASE("ModelInstance model copy constructor") {
    Context context = {"Title", {1, 1}, nullptr, {{WindowHint::Hint::Visible, false}}};
    Assets assets {"../assets"};
    assets.load(context);
    Camera camera {{1, 1}};

    assets.models.add("backpack", ModelLoader::loadModel(assets, "../assets/models/backpack/backpack.obj", {{
        ::ModelLoaderOption::FlipUV
    }}));

    ModelInstance instance = ModelInstance(assets.models.at("backpack"));
    instance.setPosition({0.5f, 0.1f, 0.2f})
            .setRotation(glm::vec3{1.0f, 0.5f, 1.0f})
            .setScale(glm::vec3{0.5f});

    instance.update(context, camera);

    ModelInstance copy = instance;

    REQUIRE(instance.getId() != copy.getId());
    REQUIRE(instance.getAbstractModel().getName() == copy.getAbstractModel().getName());
    REQUIRE(instance.getFinalMatrix() == copy.getFinalMatrix());
    REQUIRE(instance.getModelMatrix() == copy.getModelMatrix());
    REQUIRE(instance.getTransformationMatrix() == copy.getTransformationMatrix());
    REQUIRE(instance.getInstanceType() == copy.getInstanceType());
    REQUIRE(instance.getPosition() == copy.getPosition());
    REQUIRE(instance.getRotation() == copy.getRotation());
    REQUIRE(instance.getScale() == copy.getScale());
    REQUIRE(instance.doesCastShadow() == copy.doesCastShadow());
    REQUIRE(instance.isOutlined() == copy.isOutlined());
    REQUIRE(instance.isHidden() == copy.isHidden());
    REQUIRE(instance.isKilled() == copy.isKilled());

    check_opengl_state();
}

TEST_CASE("ModelInstance clone") {
    Context context = {"Title", {1, 1}, nullptr, {{WindowHint::Hint::Visible, false}}};
    Assets assets {"../assets"};
    assets.load(context);
    Camera camera {{1, 1}};

    assets.models.add("backpack", ModelLoader::loadModel(assets, "../assets/models/backpack/backpack.obj", {{
            ::ModelLoaderOption::FlipUV
    }}));

    ModelInstance instance = ModelInstance(assets.models.at("backpack"));
    instance.setPosition({0.5f, 0.1f, 0.2f})
            .setRotation(glm::vec3{1.0f, 0.5f, 1.0f})
            .setScale(glm::vec3{0.5f});

    instance.update(context, camera);

    std::unique_ptr<Instance> copy = instance.clone();

    REQUIRE(instance.getId() != copy->getId());
    REQUIRE(instance.getFinalMatrix() == copy->getFinalMatrix());
    REQUIRE(instance.getModelMatrix() == copy->getModelMatrix());
    REQUIRE(instance.getTransformationMatrix() == copy->getTransformationMatrix());
    REQUIRE(instance.getInstanceType() == copy->getInstanceType());
    REQUIRE(instance.getPosition() == copy->getPosition());
    REQUIRE(instance.getRotation() == copy->getRotation());
    REQUIRE(instance.getScale() == copy->getScale());
    REQUIRE(instance.doesCastShadow() == copy->doesCastShadow());
    REQUIRE(instance.isOutlined() == copy->isOutlined());
    REQUIRE(instance.isHidden() == copy->isHidden());
    REQUIRE(instance.isKilled() == copy->isKilled());

    check_opengl_state();
}

TEST_CASE("ModelInstance get meshes") {
    Context context = {"Title", {1, 1}, nullptr, {{WindowHint::Hint::Visible, false}}};
    Assets assets {"../assets"};
    assets.load(context);
    Camera camera {{1, 1}};

    assets.models.add("backpack", ModelLoader::loadModel(assets, "../assets/models/backpack/backpack.obj", {{
    ::ModelLoaderOption::FlipUV
    }}));

    ModelInstance instance = ModelInstance(assets.models.at("backpack"));

    REQUIRE(instance.getId() != 0);
    REQUIRE(!instance.getMeshes().empty());
    REQUIRE_NOTHROW(instance["...assets.models.backpack.backpack.obj.BezierCurve.003__0"]);
    REQUIRE_THROWS_AS(instance["shrek"], no_such_mesh);

    REQUIRE_NOTHROW(instance[0]);
    REQUIRE_THROWS_AS(instance[123], no_such_mesh);

    check_opengl_state();
}
