#include <core/context_observer.hpp>
#include <texture_loader.hpp>
#include <model_instance.hpp>
#include <material_system/material_builder.hpp>
#include <camera.hpp>
#include <scene.hpp>
#include <render.hpp>
#include <assets.hpp>
#include <skeletal_instance.hpp>
#include <shader_storage.hpp>
#include <effect_instance.hpp>
#include <elementary_instance.hpp>
#include <particle_system/effect_builder.hpp>
#include <model_loader.hpp>
#include <util/math.hpp>
#include <iostream>
#include <util/thread_pool.hpp>
#include <asset_loader.hpp>

using namespace GraphicsEngine;

class Game : public MouseMoveObserver, public KeyObserver {
private:
    ContextEventObserver context;
    Scene scene;
    Camera camera;
    Renderer render;

    ElementaryInstance* instance {nullptr};

    bool done {false};
    static constexpr glm::uvec2 window_size {1920 , 1080};
public:
    Game() : context{"Features", window_size, {{ WindowHint::Resizable, true }}}, camera{window_size}, render{context} {
        camera.setPosition({0.0f, 0.0f, 0.0f});

        context.makeCurrent();
        context.setWindowIcon(TextureLoader::loadGLFWImage(ASSETS_DIR "icons/demo.png"));
        context.setCursorMode(CursorMode::Disabled);
        context.setSwapInterval(1);
        context.setStickyKeys(true);

        context.registerObserver(static_cast<KeyObserver*>(this));
        context.registerObserver(static_cast<MouseMoveObserver*>(this));

        assets.load();
        shader_storage.initialize();

        loadScene();

        //loadingSceneConcurrently();

        scene.setSkybox("skybox");

        scene.add<ModelInstance>(assets.models.at("backpack"), glm::vec3{2.5f, 0.5f, 5.0f}, glm::vec3{ 0.0f, pi, 0.0f}, glm::vec3{0.4f});
        scene.add<ModelInstance>(assets.models.at("nanosuit"), glm::vec3{4.0f, 0.0f, 5.0f}, glm::vec3{ 0.0f, pi, 0.0f }, glm::vec3{0.1f});
        scene.add<ModelInstance>(assets.models.at("cyborg"), glm::vec3{5.0f, 0.0f, 5.0f}, glm::vec3{ 0.0f, pi, 0.0f }, glm::vec3{0.35f});

        auto& model = scene.add<SkeletalInstance>(assets.models.at("bob"), glm::vec3{ 6.0f, 0.0f, 5.0f })
                           .setScale(glm::vec3{0.02f})
                           .setRotation({ 0.0f, 0.0f, pi });
        static_cast<SkeletalInstance&>(model).play("");

        scene.add<ElementaryInstance>(assets.models.at("sphere"), assets.materials.at("material1"), glm::vec3{0.0f, 0.0f, 0.0f});
        scene.add<ElementaryInstance>(assets.models.at("sphere"), assets.materials.at("material2"), glm::vec3{ 2.0f, 0.0f, 0.0f });
        scene.add<ElementaryInstance>(assets.models.at("sphere"), assets.materials.at("material3"), glm::vec3{ 4.0f, 0.0f, 0.0f });
        scene.add<ElementaryInstance>(assets.models.at("sphere"), assets.materials.at("material4"), glm::vec3{ 6.0f, 0.0f, 0.0f });
        scene.add<ElementaryInstance>(assets.models.at("sphere"), assets.materials.at("material5"), glm::vec3{ 8.0f, 0.0f, 0.0f });
        scene.add<ElementaryInstance>(assets.models.at("sphere"), assets.materials.at("material6"), glm::vec3{ 10.0f, 0.0f, 0.0f });

        scene.lighting.point_lights.emplace_back(glm::vec4{8.0f, 0.0f, 2.0f, 1.0f}, glm::vec4{8.3f, 8.1f, 8.7f, 10.5f}, 8.0f);
        scene.lighting.point_lights.emplace_back(glm::vec4{12.0f, 0.0f, 2.0f, 1.0f}, glm::vec4{2.3f, 7.1f, 8.7f, 10.5f}, 3.0f);
    }

    void loadScene() {
        ModelLoader model_loader;
        assets.models.add("bob", model_loader.loadModel(ASSETS_DIR "models/boblamp/boblampclean.md5mesh"));
        assets.models.add("backpack", model_loader.loadModel(ASSETS_DIR "models/backpack/backpack.obj", true));
        assets.models.add("nanosuit", model_loader.loadModel(ASSETS_DIR "models/nanosuit/nanosuit.obj"));
        assets.models.add("cyborg", model_loader.loadModel(ASSETS_DIR "models/cyborg/cyborg.obj"));
        assets.skyboxes.add("skybox", std::make_shared<Skybox>(ASSETS_DIR "skyboxes/sky/sky.png"));

        MaterialBuilder builder;

        auto material1 = builder.create("material1")
                .add(PropertyType::Color, glm::vec4(0.7f, 0.3, 0.5f, 1.0f))
                .setShading(Shading::Lit)
                .build();

        auto material2 = builder.create("material2")
                .add(PropertyType::Diffuse, TextureLoader::load(ASSETS_DIR "textures/grass.jpg"))
                .setShading(Shading::Lit)
                .build();

        auto material3 = builder.create("material3")
                .add(PropertyType::EmissiveColor, glm::vec4(3.0f, 2.3, 1.0f, 1.0f))
                .setShading(Shading::Unlit)
                .build();

        auto material4 = builder.create("material4")
                .add(PropertyType::EmissiveMask, TextureLoader::load(ASSETS_DIR "textures/mask.jpg"))
                .add(PropertyType::EmissiveColor, glm::vec4(10.5f, 7.1f, 8.5f, 1.0f))
                .add(PropertyType::Color, glm::vec4(0.1f, 0.3f, 0.7f, 1.0f))
                .setShading(Shading::Lit)
                .build();

        auto material5 = builder.create("material5")
                .add(PropertyType::BlendMask, TextureLoader::load(ASSETS_DIR "textures/bricks.jpg"))
                .add(PropertyType::Color, glm::vec4(0.3f, 0.1f, 0.7f, 1.0f))
                .setShading(Shading::Lit)
                .build();

        auto material6 = builder.create("material6")
                .add(PropertyType::MetallicTexture, TextureLoader::load(ASSETS_DIR "textures/rustediron2_metallic.png"))
                .add(PropertyType::RoughnessTexture, TextureLoader::load(ASSETS_DIR "textures/rustediron2_roughness.png"))
                .add(PropertyType::Diffuse, TextureLoader::load(ASSETS_DIR "textures/rustediron2_basecolor.png"))
                .add(PropertyType::Normal, TextureLoader::load(ASSETS_DIR "textures/rustediron2_normal.png"))
                .setShading(Shading::Lit)
                .build();

        EffectBuilder eb;

        auto effect1 = eb.create("test_effect1")
                .createEmitter<SpriteEmitter>("test")
                .addModule<InitialVelocity>(EmitterModuleType::InitialVelocity, new RangeDistribution{glm::vec3{-5.0f}, glm::vec3{5.0f}})
                .addModule<Lifetime>(EmitterModuleType::Lifetime, new RangeDistribution(0.2f, 0.5f))
                .addModule<InitialSize>(EmitterModuleType::InitialSize, new RangeDistribution(0.0f, 50.0f))
                .addModule<SizeByLife>(EmitterModuleType::SizeByLife, new RangeDistribution(0.0f, 100.0f), -1.0f)
                .setMaterial(assets.materials["material3"])
                .setSpawnMode(EmitterSpawn::Mode::Burst)
                .setBurstCount(std::make_unique<ConstDistribution<uint32_t>>(1000))
                .setMaxCount(1000)
                .setSpawnRate(1.0f)
                .build();

        scene.add<EffectInstance>(effect1, glm::vec3{0.f, 1.f, 0.f});

        auto effect2 = eb.create("test_effect2")
                .createEmitter<MeshEmitter>("test")
                .addModule<InitialVelocity>(EmitterModuleType::InitialVelocity, new RangeDistribution{glm::vec3{-2.0f}, glm::vec3{2.0f}})
                .addModule<InitialColor>(EmitterModuleType::InitialColor, new RangeDistribution(glm::vec4{0.0f}, glm::vec4{4.0f}))
                .addModule<Lifetime>(EmitterModuleType::Lifetime, new RangeDistribution(0.5f, 1.0f))
                .addModule<InitialSize>(EmitterModuleType::InitialSize, new RangeDistribution(0.01f, 0.09f))
                .addModule<SizeByLife>(EmitterModuleType::SizeByLife, new RangeDistribution(0.01f, 0.09f), -1.0f)
                .setMaterial(assets.materials["material3"])
                .setMesh(assets.meshes["sphere_mesh"])
                .setSpawnMode(EmitterSpawn::Mode::Burst)
                .setBurstCount(std::make_unique<ConstDistribution<uint32_t>>(100))
                .setMaxCount(100)
                .setSpawnRate(1.0f)
                .build();

        scene.add<EffectInstance>(effect2, glm::vec3{0.f, 1.f, 0.f});
    }

    void loadingSceneConcurrently() {
        static AssetLoader loader{context};

        loader.loadModel("bob", ASSETS_DIR "models/boblamp/boblampclean.md5mesh");
        loader.loadModel("backpack", ASSETS_DIR "models/backpack/backpack.obj", true);
        loader.loadModel("nanosuit", ASSETS_DIR "models/nanosuit/nanosuit.obj");
        loader.loadModel("cyborg", ASSETS_DIR "models/cyborg/cyborg.obj");

        auto m1 = [] () {
            MaterialBuilder builder;
            builder.create("material1")
                   .add(PropertyType::Color, glm::vec4(0.7f, 0.3, 0.5f, 1.0f))
                   .setShading(Shading::Lit)
                   .build();
        };

        auto m2 = [] () {
            MaterialBuilder builder;
            builder.create("material2")
                    .add(PropertyType::Diffuse, TextureLoader::load(ASSETS_DIR "textures/grass.jpg"))
                    .setShading(Shading::Lit)
                    .build();
        };

        auto m3 = [] () {
            MaterialBuilder builder;
            builder.create("material3")
                    .add(PropertyType::EmissiveColor, glm::vec4(3.0f, 2.3, 1.0f, 1.0f))
                    .setShading(Shading::Unlit)
                    .build();
        };

        auto m4 = [] () {
            MaterialBuilder builder;
            builder.create("material4")
                    .add(PropertyType::EmissiveMask, TextureLoader::load(ASSETS_DIR "textures/mask.jpg"))
                    .add(PropertyType::EmissiveColor, glm::vec4(10.5f, 7.1f, 8.5f, 1.0f))
                    .add(PropertyType::Color, glm::vec4(0.1f, 0.3f, 0.7f, 1.0f))
                    .setShading(Shading::Lit)
                    .build();
        };

        auto m5 = [] () {
            MaterialBuilder builder;
            builder.create("material5")
                    .add(PropertyType::BlendMask, TextureLoader::load(ASSETS_DIR "textures/bricks.jpg"))
                    .add(PropertyType::Color, glm::vec4(0.3f, 0.1f, 0.7f, 1.0f))
                    .setShading(Shading::Lit)
                    .build();
        };

        auto m6 = [] () {
            MaterialBuilder builder;
            builder.create("material6")
                    .add(PropertyType::MetallicTexture, TextureLoader::load(ASSETS_DIR "textures/rustediron2_metallic.png"))
                    .add(PropertyType::RoughnessTexture, TextureLoader::load(ASSETS_DIR "textures/rustediron2_roughness.png"))
                    .add(PropertyType::Diffuse, TextureLoader::load(ASSETS_DIR "textures/rustediron2_basecolor.png"))
                    .add(PropertyType::Normal, TextureLoader::load(ASSETS_DIR "textures/rustediron2_normal.png"))
                    .setShading(Shading::Lit)
                    .build();
        };

        auto m7 = [] () {
            assets.skyboxes.add("skybox", std::make_shared<Skybox>(ASSETS_DIR "skyboxes/sky/sky.png"));
        };

        loader.build(std::move(m1));
        loader.build(std::move(m2));
        loader.build(std::move(m3));
        loader.build(std::move(m4));
        loader.build(std::move(m5));
        loader.build(std::move(m6));
        loader.build(std::move(m7));

        while (!loader.isDone()) {
            context.clearColor({0.3f, 0.3f, 0.3f, 1.0f});

            // loading screen ;)

            context.swapBuffers();
        }
        loader.delayed_work();
    }

    void onMouseMove(glm::dvec2 pos) override {
        static glm::dvec2 last_move = { 0, 0 };

        auto offset = glm::vec2{ pos.x - last_move.x, last_move.y - pos.y };
        last_move = pos;

        camera.mouseMove(offset);
    }

    void onKey(int key, [[maybe_unused]] int scancode, InputState state, [[maybe_unused]] Modifier modifier) override {
        if (key == GLFW_KEY_ESCAPE && state == InputState::Pressed) {
            done = true;
        }
    }

    void handleInput(float delta) noexcept {
        if (context.isPressed(GLFW_KEY_W)) {
            camera.movement(CameraMovement::Forward, delta);
        }

        if (context.isPressed(GLFW_KEY_S)) {
            camera.movement(CameraMovement::Backward, delta);
        }

        if (context.isPressed(GLFW_KEY_A)) {
            camera.movement(CameraMovement::Left, delta);
        }

        if (context.isPressed(GLFW_KEY_D)) {
            camera.movement(CameraMovement::Right, delta);
        }

        if (context.isPressed(GLFW_KEY_1)) {
            if (instance)
                instance->setPosition(instance->getPosition() + glm::vec3{ 0.1f, 0.0f, 0.0f});
        }

        if (context.isPressed(GLFW_KEY_2)) {
            if (instance)
                instance->setPosition(instance->getPosition() + glm::vec3{ -0.1f, 0.0f, 0.0f});
        }

        if (context.isPressed(GLFW_KEY_3)) {
            if (instance)
                scene.remove(instance->getId());
        }
    }

    void gameLoop() {
        using namespace std::chrono;
        while (!context.shouldClose() && !done) {
            static auto last_time = steady_clock::now();
            auto time = steady_clock::now();
            auto delta = duration_cast<duration<float>>(time - last_time).count();
            last_time = time;

            render.draw(context, scene, camera);

            handleInput(delta);
            context.pollEvents();
            context.swapBuffers();
        }
    }
};

int main() {
    Game game;
    game.gameLoop();

    return 0;
}