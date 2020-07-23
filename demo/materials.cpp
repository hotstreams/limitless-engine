#include <context_observer.hpp>
#include <texture_loader.hpp>
#include <model_instance.hpp>
#include <material_builder.hpp>
#include <camera.hpp>
#include <scene.hpp>
#include <render.hpp>
#include <assets.hpp>

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
    Game() : context{"Cubes demo", window_size, {{ WindowHint::Samples, 32 }}}, camera{window_size} {
        camera.setPosition({0.0f, 0.0f, 0.0f});

        context.makeCurrent();
        context.setWindowIcon(TextureLoader::loadGLFWImage(ASSETS_DIR "icons/demo.png"));
        context.setCursorMode(CursorMode::Disabled);
        context.setSwapInterval(1);
        context.setStickyKeys(true);

        context.registerObserver(static_cast<KeyObserver*>(this));
        context.registerObserver(static_cast<MouseMoveObserver *>(this));

        assets.load();

        scene.addInstance(new ModelInstance(assets.models.get("test"), glm::vec3{ 0.0f, 0.0f, 0.0f })).setScale(glm::vec3{0.001f});

        auto light = PointLight{ glm::vec4{0.0f, 1.0f, 0.0f, 1.0f}, glm::vec4{2.2f, 0.5f, 3.0f, 3.5f}, 10.0f, 0.7f, 1.8f, 2.0f};
        scene.lighting.dynamic.points_lights.add(light);
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