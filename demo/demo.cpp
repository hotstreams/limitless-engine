#include <limitless/core/context_observer.hpp>
#include <limitless/loaders/texture_loader.hpp>
#include <limitless/camera.hpp>
#include <limitless/scene.hpp>
#include <limitless/pipeline/renderer.hpp>
#include <limitless/instances/skeletal_instance.hpp>
#include <limitless/fx/effect_builder.hpp>
#include <limitless/util/math.hpp>
#include <limitless/serialization/effect_serializer.hpp>
#include <limitless/loaders/effect_loader.hpp>
#include <limitless/skybox/skybox.hpp>
#include <limitless/loaders/model_loader.hpp>
#include <limitless/text/font_atlas.hpp>
#include <limitless/text/text_instance.hpp>
#include <limitless/fx/emitters/beam_emitter.hpp>
#include <limitless/fx/emitters/sprite_emitter.hpp>
#include <limitless/instances/effect_instance.hpp>
#include <limitless/ms/material_builder.hpp>
#include <limitless/loaders/asset_loader.hpp>
#include <limitless/assets.hpp>
#include <limitless/pipeline/forward.hpp>
#include <limitless/instances/instanced_instance.hpp>
#include <limitless/ms/material.hpp>
#include <limitless/loaders/material_loader.hpp>
#include <limitless/fx/modules/mesh_location.hpp>
#include <limitless/fx/modules/mesh_location_attachment.hpp>

using namespace Limitless;

class Game : public MouseMoveObserver, public KeyObserver, public FramebufferObserver {
private:
    ContextEventObserver context;
    Scene scene;
    Camera camera;
    Renderer render;

    Assets assets;

    bool done {};
    static constexpr glm::uvec2 window_size {800, 800};

    std::unique_ptr<TextInstance> text;
    EffectInstance* effect {};
    ModelInstance* bob;
public:
    Game()
        : context {"Limitless-demo", window_size, {{ WindowHint::Resizable, true }}}
        , scene {context}
        , camera {window_size}
        , render {context}
        , assets {ENGINE_ASSETS_DIR}
{
        camera.setPosition({7.0f, 0.0f, 3.0f});

        if (!Limitless::ContextEventObserver::checkMinimumRequirements()) {
            throw std::runtime_error("Minimum requirements are not met!");
        }

        context.makeCurrent();
        context.setCursorMode(CursorMode::Normal);
        context.setSwapInterval(1);
        context.setStickyKeys(true);

        context.registerObserver(static_cast<KeyObserver*>(this));
        context.registerObserver(static_cast<MouseMoveObserver*>(this));
        context.registerObserver(static_cast<FramebufferObserver*>(this));

        assets.load(context);

        addModels();
        addSpheres();
        addEffects();

        assets.compileShaders(context, render.getSettings());

        text = std::make_unique<TextInstance>("Limitless-engine", glm::vec2{20.0f, window_size.y - 40.0f}, assets.fonts.at("nunito"));
        text->setColor({0.1f, 0.8f, 0.4f, 1.0f});

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

    // currently not working everywhere
    // some hidden tricky-bugs of context sharing there
//    void addModelsT() {
//        AssetManager manager {context, assets};
//        const fs::path assets_dir {ASSETS_DIR};
//
//        manager.loadModel("bob", assets_dir / "models/boblamp/boblampclean.md5mesh");
//        manager.loadModel("backpack", assets_dir / "models/backpack/backpack.obj", {ModelLoaderFlag::FlipUV});
//        manager.loadModel("nanosuit", assets_dir / "models/nanosuit/nanosuit.obj");
//        manager.loadModel("cyborg", assets_dir / "models/cyborg/cyborg.obj");
//
////        manager.wait();
//        while (!manager) {
//            context.clearColor({0.3f, 0.3f, 0.3f, 1.0f});
//
//            // loading screen ;)
//
//            context.swapBuffers();
//        }
//        manager.delayed_job();
//
//        assets.skyboxes.add("skybox", std::make_shared<Skybox>(context, assets, render.getSettings(), assets_dir / "skyboxes/sky/sky.png", TextureLoaderFlags{TextureLoaderFlag::TopLeftOrigin}));
//        assets.fonts.add("nunito", std::make_shared<FontAtlas>(assets_dir / "fonts/nunito.ttf", 48));
//
//        scene.setSkybox(assets.skyboxes.at("skybox"));
//        scene.add<ModelInstance>(assets.models.at("backpack"), glm::vec3{2.5f, 0.5f, 5.0f})
//            .setRotation(glm::vec3{0.0f, pi, 0.0f})
//            .setScale(glm::vec3{0.4f});
//
//        scene.add<ModelInstance>(assets.models.at("nanosuit"), glm::vec3{4.0f, 0.0f, 5.0f})
//            .setRotation(glm::vec3{ 0.0f, pi, 0.0f })
//            .setScale(glm::vec3{0.1f});
//
//        scene.add<ModelInstance>(assets.models.at("cyborg"), glm::vec3{5.0f, 0.0f, 5.0f})
//            .setRotation(glm::vec3{ 0.0f, pi, 0.0f })
//            .setScale(glm::vec3{0.35f});
//
//        scene.add<SkeletalInstance>(assets.models.at("bob"), glm::vec3{ 6.0f, 0.0f, 5.0f })
//            .setScale(glm::vec3{0.02f})
//            .setRotation(glm::vec3{ 0.0f, 0.0f, pi })
//            .play("");
//    }

    void addModels() {
        ModelLoader model_loader{assets};
        const fs::path assets_dir{ASSETS_DIR};

        assets.models.add("bob", model_loader.loadModel(assets_dir / "models/boblamp/boblampclean.md5mesh"));
        assets.models.add("backpack", model_loader.loadModel(assets_dir / "models/backpack/backpack.obj", {ModelLoaderFlag::FlipUV}));
        assets.models.add("nanosuit", model_loader.loadModel(assets_dir / "models/nanosuit/nanosuit.obj"));
        assets.models.add("cyborg", model_loader.loadModel(assets_dir / "models/cyborg/cyborg.obj"));

        assets.skyboxes.add("skybox", std::make_shared<Skybox>(assets, assets_dir / "skyboxes/sky/sky.png", TextureLoaderFlags{TextureLoaderFlag::TopLeftOrigin}));
        scene.setSkybox(assets.skyboxes.at("skybox"));

        assets.fonts.add("nunito", std::make_shared<FontAtlas>(assets_dir / "fonts/nunito.ttf", 48));

        scene.add<ModelInstance>(assets.models.at("backpack"), glm::vec3{2.5f, 0.5f, 5.0f})
                .setRotation(glm::vec3{0.0f, PI, 0.0f})
                .setScale(glm::vec3{0.4f});

        scene.add<ModelInstance>(assets.models.at("nanosuit"), glm::vec3{4.0f, 0.0f, 5.0f})
                .setRotation(glm::vec3{0.0f, PI, 0.0f})
                .setScale(glm::vec3{0.1f});

        scene.add<ModelInstance>(assets.models.at("cyborg"), glm::vec3{5.0f, 0.0f, 5.0f})
                .setRotation(glm::vec3{0.0f, PI, 0.0f})
                .setScale(glm::vec3{0.35f});

        bob = &scene.add<SkeletalInstance>(assets.models.at("bob"), glm::vec3{6.0f, 0.0f, 5.0f})
                .setScale(glm::vec3{0.02f})
                .setRotation(glm::vec3{0.0f, 0.0f, PI})
                .play("");

        ms::MaterialBuilder builder{assets};

        builder.setName("test")
                .add(ms::Property::Color, glm::vec4(2.0f, 1.0f, 1.0f, 1.0f))
                .setModelShaders({ModelShader::Instanced})
                .setShading(ms::Shading::Unlit)
                .build();

        auto& instanced = scene.add<InstancedInstance<ModelInstance>>(glm::vec3(0.0f));
        for (uint32_t i = 0; i < 10; ++i) {
            instanced.addInstance(std::make_unique<ModelInstance>(assets.models.at("sphere"), assets.materials.at("test"), glm::vec3{-4.0, 1.0f, -8.0 + i * 1.9}));
        }
    }

    void addSpheres() {
        ms::MaterialBuilder builder {assets};
        TextureLoader tex_loader {assets};

        const fs::path assets_dir {ASSETS_DIR};

        context.setWindowIcon(tex_loader.loadGLFWImage(assets_dir / "icons/demo.png"));

        builder.setName("Color")
                .add(ms::Property::Color, glm::vec4(0.7f, 0.3, 0.5f, 1.0f))
                .setShading(ms::Shading::Lit)
                .build();
        scene.add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("Color"), glm::vec3{10.0f, 1.0f, 0.0f});

        builder.setName("Diffuse")
                .add(ms::Property::Diffuse, tex_loader.load(assets_dir / "textures/triangle.jpg"))
                .setShading(ms::Shading::Lit)
                .build();
        scene.add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("Diffuse"), glm::vec3{10.0f, 1.0f, 2.0f });

        builder.setName("EmissiveColor")
                .add(ms::Property::EmissiveColor, glm::vec4(3.0f, 0.1f, 3.0f, 1.0f))
                .setShading(ms::Shading::Unlit)
                .build();
        scene.add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("EmissiveColor"), glm::vec3{10.0f, 1.0f, 4.0f });

        builder.setName("BlendMask")
                .add(ms::Property::BlendMask, tex_loader.load(assets_dir / "textures/bricks.jpg", {TextureLoaderFlag::BottomLeftOrigin,
                                                                                                   TextureLoaderFlag::NearestFilter,
                                                                                                   TextureLoaderFlag::MipMap,
                                                                                                   TextureLoaderFlag::WrapRepeat}))
                .add(ms::Property::Color, glm::vec4(0.3f, 0.1f, 0.7f, 1.0f))
                .setShading(ms::Shading::Lit)
                .setTwoSided(true)
                .build();
        scene.add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("BlendMask"), glm::vec3{10.0f, 1.0f, 6.0f });

        builder.setName("PBR")
                .add(ms::Property::MetallicTexture, tex_loader.load(assets_dir / "textures/rustediron2_metallic.png"))
                .add(ms::Property::RoughnessTexture, tex_loader.load(assets_dir / "textures/rustediron2_roughness.png"))
                .add(ms::Property::Diffuse, tex_loader.load(assets_dir / "textures/rustediron2_basecolor.png"))
                .add(ms::Property::Normal, tex_loader.load(assets_dir / "textures/rustediron2_normal.png"))
                .setShading(ms::Shading::Lit)
                .build();
        scene.add<ModelInstance>(assets.models.at("sphere"), assets.materials.at("PBR"), glm::vec3{10.0f, 1.0f, 8.0f });

        builder.setName("floor")
                .setShading(ms::Shading::Lit)
                .add(ms::Property::Diffuse, tex_loader.load(assets_dir / "textures/wood.jpg"))
                .setShading(ms::Shading::Lit)
                .setTwoSided(true)
                .build();

        scene.add<ModelInstance>(assets.models.at("plane"), assets.materials.at("floor"), glm::vec3{3.0f, 0.f, 0.0f})
                .setScale(glm::vec3{20.0f, 1.0f, 20.0f});
    }

    void addEffects() {
        fx::EffectBuilder builder {assets};

        builder.create("mesh_test")
               .createEmitter<fx::SpriteEmitter>("sparks")
               .addLifetime(std::make_unique<RangeDistribution<float>>(1.0f, 2.0f))
//               .addLifetime(std::make_unique<ConstDistribution<float>>(9999.0f))
               .addInitialSize(std::make_unique<ConstDistribution<float>>(5.0f))
               .addInitialColor(std::make_unique<RangeDistribution<glm::vec4>>(glm::vec4{0.0f}, glm::vec4{2.0f}))
//               .addInitialMeshLocation(assets.models.at("bob"))
               .addMeshLocationAttachment(assets.models.at("bob"))
               .setMaterial(assets.materials.at("EmissiveColor"))
               .setMaxCount(10000)
               .setSpawnRate(5000)
               .build();

        builder.create("effect1")
                .createEmitter<fx::SpriteEmitter>("generate")
                .addInitialVelocity(std::make_unique<RangeDistribution<glm::vec3>>(glm::vec3{-5.0f, 0.0f, -5.0f}, glm::vec3{5.0f}))
                .addInitialAcceleration(std::make_unique<RangeDistribution<glm::vec3>>(glm::vec3{0.0f}, glm::vec3{0.0f, 5.0f, 0.0f}))
                .addLifetime(std::make_unique<RangeDistribution<float>>(0.2f, 0.5f))
                .addInitialSize(std::make_unique<RangeDistribution<float>>(1.0f, 25.0f))
                .addSizeByLife(std::make_unique<ConstDistribution<float>>(0.0f), 1.0f)
                .addInitialColor(std::make_unique<RangeDistribution<glm::vec4>>(glm::vec4{0.0f}, glm::vec4{2.0f}))
                .setMaterial(assets.materials.at("EmissiveColor"))
                .setSpawnMode(fx::EmitterSpawn::Mode::Burst)
                .setBurstCount(std::make_unique<ConstDistribution<uint32_t>>(100))
                .setMaxCount(100)
                .setSpawnRate(1.0f)
                .setLocalSpace(true)
                .build();

        effect = &scene.add<EffectInstance>(assets.effects.at("effect1"), glm::vec3{2.f, 1.f, 8.f});
        auto& e = scene.add<EffectInstance>(assets.effects.at("effect1"), glm::vec3{4.f, 1.f, 8.f});
        scene.add<EffectInstance>(assets.effects.at("effect1"), glm::vec3{6.f, 1.f, 8.f});

        e.get<fx::SpriteEmitter>("generate").getMaterial().getEmissiveColor().setValue(glm::vec4(2.0f));

        builder.create("effect2")
                .createEmitter<fx::MeshEmitter>("generate")
                .addInitialVelocity(std::make_unique<RangeDistribution<glm::vec3>>(glm::vec3{-5.0f, 0.0f, -5.0f}, glm::vec3{5.0f}))
                .addInitialAcceleration(std::make_unique<RangeDistribution<glm::vec3>>(glm::vec3{0.0f}, glm::vec3{0.0f, 5.0f, 0.0f}))
                .addLifetime(std::make_unique<RangeDistribution<float>>(0.2f, 0.5f))
                .addInitialSize(std::make_unique<ConstDistribution<glm::vec3>>(glm::vec3(1.0f)))
                .addInitialColor(std::make_unique<RangeDistribution<glm::vec4>>(glm::vec4{0.0f}, glm::vec4{2.0f}))
                .setMaterial(assets.materials.at("EmissiveColor"))
                .setMesh(assets.meshes.at("sphere"))
                .setSpawnMode(fx::EmitterSpawn::Mode::Burst)
                .setBurstCount(std::make_unique<ConstDistribution<uint32_t>>(100))
                .setMaxCount(100)
                .setSpawnRate(1.0f)
                .setLocalSpace(true)
                .build();

//        EffectLoader::save(assets.getBaseDir() / "effects/test2", assets.effects.at("effect2"));
//        EffectLoader::load(context, assets, RenderSettings{}, assets.getBaseDir() / "effects/test2");

        effect = &scene.add<EffectInstance>(assets.effects.at("effect2"), glm::vec3{5.f, 1.f, -5.f});

        fx::EffectBuilder beam_builder{assets};
        beam_builder.create("test_beam")
                     .createEmitter<fx::BeamEmitter>("test")
                     .setMaterial(assets.materials.at("EmissiveColor"))
                     .addLifetime(std::make_unique<ConstDistribution<float>>(1.0f))
                     .setMaxCount(1)
                     .build();

//        EffectLoader::save(assets.getBaseDir() / "effects/test3", assets.effects.at("test_beam"));
//        EffectLoader::load(context, assets, RenderSettings{}, assets.getBaseDir() / "effects/test3");

        scene.add<EffectInstance>(assets.effects.at("test_beam"), glm::vec3{8.0f, 2.0f, 8.0f});

//        const auto& module = scene.add<EffectInstance>(assets.effects.at("mesh_test"), glm::vec3{0.0f, 0.0f, 0.0f})
//            .get<fx::SpriteEmitter>("sparks")
//            .getModule(fx::ModuleType::MeshLocationAttachment);
//
//        static_cast<fx::MeshLocationAttachment<fx::SpriteParticle>&>(*module)
//        .attachModelInstance(bob);

        Limitless::ms::MaterialBuilder materialBuilder {assets};
        TextureLoader loader {assets};
        const fs::path assets_dir {ASSETS_DIR};

        materialBuilder .setName("fireball_material")
                .setFragmentSnippet("uv.y -= 0.1;\n"
                                    "vec2 panned = vec2(uv.x + 0.5 + in_data.properties.x, uv.y + 0.8 + in_data.properties.y);\n"
                                    "uv += texture(noise, panned).g * 0.3;\n"
                                    "\n"
                                    "vec2 offset_panned1 = vec2(uv.x + 0.66, uv.y + 0.33);\n"
                                    "float offset1 = texture(noise, offset_panned1).r;\n"
                                    "\n"
                                    "vec2 offset_panned2 = vec2(uv.x + 0.45, uv.y + 0.71);\n"
                                    "float offset2 = texture(noise, offset_panned2).r;\n"
                                    "\n"
                                    "float r = offset1 * offset2;\n"
                                    "\n"
                                    "mat_diffuse = texture(material_diffuse, uv);\n"
                                    "mat_diffuse.rgb *= clamp((mat_diffuse.a - in_data.properties.z) * r, 0, 1);")
//			.addUniform(std::make_unique<UniformTime>("time"))
                .addUniform(std::make_unique<UniformSampler>("noise", loader.load(assets_dir /  "textures/true_noise.tga")))
        .add(Limitless::ms::Property::Diffuse, loader.load(assets_dir /  "textures/true_fire.tga"))
        .add(Limitless::ms::Property::EmissiveColor, glm::vec4{10.0f, 3.0f, 1.0f, 1.0f})
                .setShading(Limitless::ms::Shading::Unlit)
                .setBlending(Limitless::ms::Blending::Additive)
                .addModelShader(ModelShader::Effect)
                .build();

        Limitless::fx::EffectBuilder eb {assets};
        eb.create("fireball")
                .createEmitter<Limitless::fx::SpriteEmitter>("test")
                .addMeshLocationAttachment(assets.models.at("bob"))
                .addInitialRotation(std::make_unique<RangeDistribution<glm::vec3>>(glm::vec3{0.0f, 0.0f, 0.0f}, glm::vec3{6.28f, 0.0f, 0.0f}))
                .addLifetime(std::make_unique<RangeDistribution<float>>(0.5f, 1.0f))
                .addInitialSize(std::make_unique<ConstDistribution<float>>(64.0f))
                .addSizeByLife(std::make_unique<RangeDistribution<float>>(256.0f, 512.0f), -1.0f)
                .addCustomMaterial(std::make_unique<RangeDistribution<float>>(0.0f, 0.9f),
                                   std::make_unique<RangeDistribution<float>>(0.0f, 0.9f),
                                   std::make_unique<ConstDistribution<float>>(0.0f),
                                   nullptr)
                .addCustomMaterialByLife(nullptr,
                                         nullptr,
                                         std::make_unique<ConstDistribution<float>>(1.0f),
                                         nullptr)
                .setMaterial(assets.materials.at("fireball_material"))
                .setSpawnMode(Limitless::fx::EmitterSpawn::Mode::Spray)
                .setMaxCount(1000)
                .setSpawnRate(500.0f)
                .build();


        const auto& module = scene.add<EffectInstance>(assets.effects.at("fireball"), glm::vec3{0.0f, 0.0f, 0.0f})
                .get<fx::SpriteEmitter>("test")
                .getModule(fx::ModuleType::MeshLocationAttachment);

        static_cast<fx::MeshLocationAttachment<fx::SpriteParticle>&>(*module)
                .attachModelInstance(bob);
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

        if (key == GLFW_KEY_1 && state == InputState::Pressed) {
            render.getSettings().physically_based_render = !render.getSettings().physically_based_render;
            render.update(context, assets, scene);
        }

        if (key == GLFW_KEY_2 && state == InputState::Pressed) {
            render.getSettings().normal_mapping = !render.getSettings().normal_mapping;
            render.update(context, assets, scene);
        }

        if (key == GLFW_KEY_3 && state == InputState::Pressed) {
            render.getSettings().shading_model = render.getSettings().shading_model == ShadingModel::BlinnPhong ? ShadingModel::Phong : ShadingModel::BlinnPhong;
            render.update(context, assets, scene);
        }

        if (key == GLFW_KEY_4 && state == InputState::Pressed) {
            render.getSettings().directional_csm = !render.getSettings().directional_csm;
            render.update(context, assets, scene);
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

    void updateEffect(float delta) {
        if (effect) {
            effect->rotateBy(glm::vec3{0.0f, PI * delta * 2.0f, 0.0f});
        }
    }

    std::chrono::steady_clock::time_point last_time = std::chrono::steady_clock::now();
    void gameLoop() {
        using namespace std::chrono;
        while (!context.shouldClose() && !done) {
            auto time = steady_clock::now();
            auto delta = duration_cast<duration<float>>(time - last_time).count();
            last_time = time;

            updateEffect(delta);

            render.draw(context, assets, scene, camera);
            text->draw(context, assets);

            context.swapBuffers();
            context.pollEvents();
            handleInput(delta);
        }
    }
};

int main() {
    Game game;
    game.gameLoop();
    return 0;
}