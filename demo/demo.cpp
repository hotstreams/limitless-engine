#include <context_observer.hpp>
#include <texture_loader.hpp>
#include <model_instance.hpp>
#include <material_builder.hpp>
#include <camera.hpp>
#include <scene.hpp>
#include <render.hpp>
#include <assets.hpp>
#include <skeletal_instance.hpp>
#include <shader_storage.hpp>
#include <effect_instance.hpp>

#ifndef M_PI
    #define M_PI (3.14159265358979323846)
#endif

using namespace GraphicsEngine;

class Game : public MouseMoveObserver, public KeyObserver {
private:
    ContextEventObserver context;
    Scene scene;
    Camera camera;
    Renderer render;

    bool done {false};
    static constexpr glm::uvec2 window_size {1920 , 1080};
public:
    Game() : context{"Features", window_size, {{ WindowHint::Resizable, false }}}, camera{window_size}, render{context} {
        camera.setPosition({0.0f, 0.0f, 0.0f});

        context.makeCurrent();
        context.setWindowIcon(TextureLoader::loadGLFWImage(ASSETS_DIR "icons/demo.png"));
        context.setCursorMode(CursorMode::Disabled);
        context.setSwapInterval(1);
        context.setStickyKeys(true);

        context.registerObserver(static_cast<KeyObserver*>(this));
        context.registerObserver(static_cast<MouseMoveObserver*>(this));

        assets.load();

        MaterialBuilder builder;

        auto material1 = builder.add(PropertyType::Color, glm::vec4(0.7f, 0.3, 0.5f, 1.0f))
                                .setShading(Shading::Lit)
                                .build("material1");

        auto material2 = builder.add(PropertyType::Diffuse, TextureLoader::load(ASSETS_DIR "textures/grass.jpg"))
                                .setShading(Shading::Lit)
                                .build("material2");

        auto material3 = builder.add(PropertyType::EmissiveColor, glm::vec4(3.0f, 2.3, 1.0f, 1.0f))
                                .setShading(Shading::Unlit)
                                .build("material3");

        auto material4 = builder.add(PropertyType::EmissiveMask, TextureLoader::load(ASSETS_DIR "textures/mask.jpg"))
                                .add(PropertyType::EmissiveColor, glm::vec4(10.5f, 7.1f, 8.5f, 1.0f))
                                .add(PropertyType::Color, glm::vec4(0.1f, 0.3f, 0.7f, 1.0f))
                                .setShading(Shading::Lit)
                                .build("material4");

        auto material5 = builder.add(PropertyType::BlendMask, TextureLoader::load(ASSETS_DIR "textures/bricks.jpg"))
                                .add(PropertyType::Color, glm::vec4(0.3f, 0.1f, 0.7f, 1.0f))
                                .setShading(Shading::Lit)
                                .build("material5");

        auto material6 = builder.add(PropertyType::MetallicTexture, TextureLoader::load(ASSETS_DIR "textures/rustediron2_metallic.png"))
                                .add(PropertyType::RoughnessTexture, TextureLoader::load(ASSETS_DIR "textures/rustediron2_roughness.png"))
                                .add(PropertyType::Diffuse, TextureLoader::load(ASSETS_DIR "textures/rustediron2_basecolor.png"))
                                .add(PropertyType::Normal, TextureLoader::load(ASSETS_DIR "textures/rustediron2_normal.png"))
                                .setShading(Shading::Lit)
                                .build("material6");

        scene.addInstance(new ModelInstance(assets.models.get("backpack"), glm::vec3{2.5f, 0.5f, 5.0f}))
             .setScale(glm::vec3(0.4f))
             .setRotation({ 0.0f, M_PI, 0.0f });

        scene.addInstance(new ModelInstance(assets.models.get("nanosuit"), glm::vec3{4.0f, 0.0f, 5.0f}))
             .setScale(glm::vec3(0.1f))
             .setRotation({ 0.0f, M_PI, 0.0f });

        scene.addInstance(new ModelInstance(assets.models.get("cyborg"), glm::vec3{5.0f, 0.0f, 5.0f}))
             .setScale(glm::vec3(0.35f))
             .setRotation({ 0.0f, M_PI, 0.0f });

        auto& model = scene.addInstance(new SkeletalInstance(assets.models.get("bob"), glm::vec3{ 6.0f, 0.0f, 5.0f }))
                           .setScale(glm::vec3{0.02f})
                           .setRotation({ 0.0f, 0.0f, M_PI });
        static_cast<SkeletalInstance&>(model).play("");

        scene.addInstance(new ModelInstance(assets.models.get("sphere"), material1, glm::vec3{ 0.0f, 0.0f, 0.0f }));
        scene.addInstance(new ModelInstance(assets.models.get("sphere"), material2, glm::vec3{ 2.0f, 0.0f, 0.0f }));
        scene.addInstance(new ModelInstance(assets.models.get("sphere"), material3, glm::vec3{ 4.0f, 0.0f, 0.0f }));
        scene.addInstance(new ModelInstance(assets.models.get("sphere"), material4, glm::vec3{ 6.0f, 0.0f, 0.0f }));
        scene.addInstance(new ModelInstance(assets.models.get("sphere"), material5, glm::vec3{ 8.0f, 0.0f, 0.0f }));
        scene.addInstance(new ModelInstance(assets.models.get("sphere"), material6, glm::vec3{ 10.0f, 0.0f, 0.0f }));

        auto light = PointLight{glm::vec4{4.0f, 0.0f, 2.0f, 1.0f}, glm::vec4{1.3f, 2.1f, 2.7f, 7.5f}, 8.0f};
        scene.lighting.dynamic.point_lights.add(light);

        scene.setSkybox("skybox");

        shader_storage.initialize();
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