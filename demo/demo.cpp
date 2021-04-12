#include <limitless/core/context_observer.hpp>
#include <limitless/loaders/texture_loader.hpp>
#include <limitless/camera.hpp>
#include <limitless/scene.hpp>
#include <limitless/renderer.hpp>
#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/instances/elementary_instance.hpp>
#include <limitless/particle_system/effect_builder.hpp>
#include <limitless/util/math.hpp>
#include <limitless/instances/text_instance.hpp>
#include <limitless/material_system/custom_material_builder.hpp>
#include <limitless/serialization/effect_serializer.hpp>
#include <limitless/loaders/effect_loader.hpp>
#include <limitless/loaders/material_loader.hpp>
#include <limitless/assets.hpp>
#include <limitless/loaders/model_loader.hpp>
#include <limitless/loaders/asset_loader.hpp>

using namespace LimitlessEngine;

class Game : public MouseMoveObserver, public KeyObserver, public FramebufferObserver {
private:
    ContextEventObserver context;
    Scene scene;
    Camera camera;
    Renderer render;

    Assets assets;

    bool done {};
    static constexpr glm::uvec2 window_size {800, 800};
public:
    Game() : context{"Limitless-demo", window_size, {{ WindowHint::Resizable, true }}}, camera{window_size}, render{context} {
        camera.setPosition({7.0f, 0.0f, 3.0f});

        context.makeCurrent();
        context.setCursorMode(CursorMode::Disabled);
        context.setSwapInterval(1);
        context.setStickyKeys(true);

        context.registerObserver(static_cast<KeyObserver*>(this));
        context.registerObserver(static_cast<MouseMoveObserver*>(this));
        context.registerObserver(static_cast<FramebufferObserver*>(this));

        assets.load(context);

        addModels();
        addSpheres();
        addEffects();

//        auto test_text = new TextInstance("Limitless-engine", {20.0f, window_size.y - 40.0f}, {1.0f, 1.0f}, {1.5f, 3.8f, 2.4f, 1.f}, assets.fonts.at("nunito"), context);
//        scene.add(test_text);

        scene.lighting.directional_light = { glm::vec4{2.0f, -5.0f, 2.0f, 1.0f}, glm::vec4{0.1f, 0.7f, 1.3f, 1.0f} };
        scene.lighting.point_lights.emplace_back(glm::vec4{-1.0f, 1.3f, 2.0f, 1.0f}, glm::vec4{2.3f, 1.1f, 1.2f, 2.0f}, 4.6f);
        scene.lighting.point_lights.emplace_back(glm::vec4{3.0f, 1.3f, 2.0f, 1.0f}, glm::vec4{1.3f, 0.3f, 2.7f, 2.0f}, 4.2f);
        scene.lighting.point_lights.emplace_back(glm::vec4{8.0f, 1.3f, 2.0f, 1.0f}, glm::vec4{1.3f, 2.3f, 1.7f, 2.0f}, 4.5f);
    }

    ~Game() override {
        context.unregisterObserver(static_cast<KeyObserver*>(this));
        context.unregisterObserver(static_cast<MouseMoveObserver*>(this));
        context.unregisterObserver(static_cast<FramebufferObserver*>(this));
    }

    void addModels() {
        ModelLoader model_loader {context, assets};

        assets.models.add("bob", model_loader.loadModel(ASSETS_DIR "models/boblamp/boblampclean.md5mesh"));
        assets.models.add("backpack", model_loader.loadModel(ASSETS_DIR "models/backpack/backpack.obj", {ModelLoaderFlag::FlipUV}));
        assets.models.add("nanosuit", model_loader.loadModel(ASSETS_DIR "models/nanosuit/nanosuit.obj"));
        assets.models.add("cyborg", model_loader.loadModel(ASSETS_DIR "models/cyborg/cyborg.obj"));
        assets.skyboxes.add("skybox", std::make_shared<Skybox>(assets, ASSETS_DIR "skyboxes/sky/sky.png"));
        assets.fonts.add("nunito", std::make_shared<FontAtlas>(ASSETS_DIR "fonts/nunito.ttf", 48));

        scene.setSkybox(assets.skyboxes.at("skybox"));
        scene.add<ModelInstance>(assets.models.at("backpack"), glm::vec3{2.5f, 0.5f, 5.0f}, glm::vec3{ 0.0f, pi, 0.0f}, glm::vec3{0.4f});
        scene.add<ModelInstance>(assets.models.at("nanosuit"), glm::vec3{4.0f, 0.0f, 5.0f}, glm::vec3{ 0.0f, pi, 0.0f }, glm::vec3{0.1f});
        scene.add<ModelInstance>(assets.models.at("cyborg"), glm::vec3{5.0f, 0.0f, 5.0f}, glm::vec3{ 0.0f, pi, 0.0f }, glm::vec3{0.35f});
        scene.add<SkeletalInstance>(assets.models.at("bob"), glm::vec3{ 6.0f, 0.0f, 5.0f })
                .setScale(glm::vec3{0.02f})
                .setRotation(glm::vec3{ 0.0f, 0.0f, pi })
                .play("");
    }

    void addSpheres() {
        MaterialBuilder builder {context, assets};
        TextureLoader tex_loader {assets};

        context.setWindowIcon(tex_loader.loadGLFWImage(ASSETS_DIR "icons/demo.png"));

        builder.create("Color")
                .add(PropertyType::Color, glm::vec4(0.7f, 0.3, 0.5f, 1.0f))
                .setShading(Shading::Lit)
                .build();
        scene.add<ElementaryInstance>(assets.models.at("sphere"), assets.materials.at("Color"), glm::vec3{0.0f, 0.0f, 0.0f});

        builder.create("Diffuse")
                .add(PropertyType::Diffuse, tex_loader.load(ASSETS_DIR "textures/triangle.jpg"))
                .setShading(Shading::Lit)
                .build();
        scene.add<ElementaryInstance>(assets.models.at("sphere"), assets.materials.at("Diffuse"), glm::vec3{ 2.0f, 0.0f, 0.0f });

        builder.create("EmissiveColor")
                .add(PropertyType::EmissiveColor, glm::vec4(3.0f, 2.3, 1.0f, 1.0f))
                .setShading(Shading::Unlit)
                .build();
        scene.add<ElementaryInstance>(assets.models.at("sphere"), assets.materials.at("EmissiveColor"), glm::vec3{ 4.0f, 0.0f, 0.0f });

        builder.create("BlendMask")
                .add(PropertyType::BlendMask, tex_loader.load(ASSETS_DIR "textures/bricks.jpg"))
                .add(PropertyType::Color, glm::vec4(0.3f, 0.1f, 0.7f, 1.0f))
                .setShading(Shading::Lit)
                .setTwoSided(true)
                .build();
        scene.add<ElementaryInstance>(assets.models.at("sphere"), assets.materials.at("BlendMask"), glm::vec3{ 6.0f, 0.0f, 0.0f });

        builder.create("PBR")
                .add(PropertyType::MetallicTexture, tex_loader.load(ASSETS_DIR "textures/rustediron2_metallic.png"))
                .add(PropertyType::RoughnessTexture, tex_loader.load(ASSETS_DIR "textures/rustediron2_roughness.png"))
                .add(PropertyType::Diffuse, tex_loader.load(ASSETS_DIR "textures/rustediron2_basecolor.png"))
                .add(PropertyType::Normal, tex_loader.load(ASSETS_DIR "textures/rustediron2_normal.png"))
                .setShading(Shading::Lit)
                .build();
        scene.add<ElementaryInstance>(assets.models.at("sphere"), assets.materials.at("PBR"), glm::vec3{ 8.0f, 0.0f, 0.0f });

        builder.create("floor")
                .setShading(Shading::Lit)
                .add(PropertyType::Diffuse, tex_loader.load(ASSETS_DIR "textures/wood.jpg"))
                .setTwoSided(true)
                .build();
        scene.add<ElementaryInstance>(assets.models.at("plane"), assets.materials.at("floor"), glm::vec3{3.0f, -1.0f, 0.0f})
                .setScale(glm::vec3{20.0f, 1.0f, 20.0f});
    }

    void addEffects() {
        EffectBuilder builder {context, assets};

        builder.create("effect1")
                .createEmitter<SpriteEmitter>("test")
                .addModule<InitialVelocity>(EmitterModuleType::InitialVelocity, new RangeDistribution{glm::vec3{-5.0f}, glm::vec3{5.0f}})
                .addModule<Lifetime>(EmitterModuleType::Lifetime, new RangeDistribution(0.2f, 0.5f))
                .addModule<InitialSize>(EmitterModuleType::InitialSize, new RangeDistribution(0.0f, 50.0f))
                .addModule<SizeByLife>(EmitterModuleType::SizeByLife, new RangeDistribution(0.0f, 100.0f), -1.0f)
                .addModule<InitialColor>(EmitterModuleType::InitialColor, new RangeDistribution(glm::vec4{}, glm::vec4{2.0f}))
                .setMaterial(assets.materials.at("EmissiveColor"))
                .setSpawnMode(EmitterSpawn::Mode::Burst)
                .setBurstCount(std::make_unique<ConstDistribution<uint32_t>>(1000))
                .setMaxCount(1000)
                .setSpawnRate(1.0f)
                .build();

        scene.add<EffectInstance>(assets.effects.at("effect1"), glm::vec3{2.f, 1.f, -5.f});
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

    void onFramebufferChange(glm::uvec2 size) override {
        camera.updateProjection(size);
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
    }

    void gameLoop() {
        using namespace std::chrono;
        while (!context.shouldClose() && !done) {
            static auto last_time = steady_clock::now();
            auto time = steady_clock::now();
            auto delta = duration_cast<duration<float>>(time - last_time).count();
            last_time = time;

            render.draw(context, assets, scene, camera);

            handleInput(delta);

            context.swapBuffers();
            context.pollEvents();
        }
    }
};

int main() {
    Game game;
    game.gameLoop();
    return 0;
}