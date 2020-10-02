#include <render.hpp>
#include <shader_storage.hpp>
#include <scene.hpp>

#include <assets.hpp>
#include <material_builder.hpp>

using namespace GraphicsEngine;

bool done = false;

class TestCube : public MouseMoveObserver, public KeyObserver {
public:
    ContextEventObserver context;
private:
    Camera camera;
    Scene scene;
    Render render;
    std::shared_ptr<ShaderProgram> shader;
    static constexpr glm::uvec2 window_size {500, 500};
public:
    TestCube() : context{"Cubes demo", window_size, {{ WindowHint::Samples, 32 }}}, camera{window_size}, render{context} {
        camera.setPosition({0.0f, 1.0f, -1.5f});
        context.makeCurrent();

        context.setCursorMode(CursorMode::Disabled);

        context.registerObserver(static_cast<KeyObserver*>(this));
        context.registerObserver(static_cast<MouseMoveObserver *>(this));

        shader_storage.initialize();
        assets.load();

        MaterialBuilder builder;

        auto mat = builder.add(PropertyType::Color, glm::vec4{0.0f, 0.3f, 0.6f, 1.0f}).build("cube material");

        scene.addInstance(new ModelInstance(assets.models.get("cube"), mat, glm::vec3{0.0f, 0.0f, 0.0f}));
    }

    void draw() {
        render.draw(context, scene, camera);
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
};

int main() {
    TestCube scene;

    while (!scene.context.shouldClose() && !done) {
        static auto last_time = glfwGetTime();
        auto time = glfwGetTime();
        auto delta = time - last_time;
        last_time = time;

        scene.context.clearColor({ 0.3f, 0.3f, 0.3f, 1.0f});
        scene.context.clear(Clear::ColorDepth);
        scene.context.setViewPort(scene.context.getSize());

        scene.draw();

        scene.handleInput(delta);
        scene.context.pollEvents();
        scene.context.swapBuffers();
    }

    return 0;
}