#include <context_observer.hpp>
#include <texture_loader.hpp>
#include <model_instance.hpp>
#include <material_builder.hpp>
#include <camera.hpp>
#include <scene.hpp>
#include <render.hpp>
#include <assets.hpp>
#include <skeletal_instance.hpp>
#include <elementary_model.hpp>
#include <editor/effect_creator.hpp>
#include <any>
#include <iostream>

using namespace GraphicsEngine;

class Game : public MouseMoveObserver, public KeyObserver {
private:
    ContextEventObserver context;
    Scene scene;
    Camera camera;
    Render render;

    bool done {false};
    static constexpr glm::uvec2 window_size {2048 , 1080};
public:
    Game() : context{"Cubes demo", window_size, {{ WindowHint::Samples, 32 }}}, camera{window_size}, render{context} {
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
        //auto mat = builder.add(PropertyType::Diffuse, TextureLoader::load(ASSETS_DIR "textures/opengl.jpg"))
        auto mat = builder.add(PropertyType::Color, glm::vec4(0.3f, 0.0, 0.0f, 1.0f))
                .setShading(Shading::Unlit)
                .setBlending(Blending::Modulate)
                .build("sphere");

        auto mat1 = builder.add(PropertyType::Color, glm::vec4(1.0f, 0.0f, 0.0f, 1.0f))
                .setShading(Shading::Unlit)
                .build("sphere1");

        auto mat2 = builder.add(PropertyType::EmissiveColor, glm::vec4(7.0f, 5.0f, 0.0f, 1.0f))
                .setShading(Shading::Unlit)
                .build("sphere12");

        //scene.addInstance(new ModelInstance(assets.models.get("sponza"), glm::vec4{0.0f, 1.0f, 0.5f, 1.0f})).setScale(glm::vec3{0.1f});
        auto& sphere = scene.addInstance(new ModelInstance(std::make_shared<Sphere>(100, 100), mat2, glm::vec3{ 0.0f, 0.0f, 0.0f })).setScale(glm::vec3{0.1f});
        //sphere.getMesh().getMaterial().apply(mat);


        auto& model = scene.addInstance(new SkeletalInstance(assets.models.get("bob"), glm::vec3{ 0.0f, 0.0f, 0.0f }))
            .setScale(glm::vec3{0.005f})
            .setRotation({ -M_PI_2, 0.0f, 0.0f});

        static_cast<SkeletalInstance&>(model).play("");

        auto light = PointLight{glm::vec4{0.0f, 1.0f, 0.0f, 1.0f}, glm::vec4{2.2f, 0.5f, 3.0f, 3.5f}, 1.0f, 0.7f, 1.8f, 2.0f};
        scene.lighting.dynamic.point_lights.add(light);

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

            //static Editor::SpriteEmitter e;
            //e.draw(Blending::Opaque);

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