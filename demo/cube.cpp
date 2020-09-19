#include <vertex_array.hpp>
#include <buffer_builder.hpp>
#include <vertex.hpp>
#include <shader_compiler.hpp>
#include <texture_loader.hpp>
#include <context_observer.hpp>
#include <camera.hpp>

using namespace GraphicsEngine;

#define ASSETS_DIR "../assets/"

class Cube {
private:
    VertexArray vertex_array;
    std::unique_ptr<Buffer> buffer;
    std::shared_ptr<Texture> texture;
public:
    Cube() noexcept {
        std::vector<Vertex> vertices = {
            // back face
            {{-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f}}, // bottom-left
            {{0.5f, -0.5f, -0.5f},  {1.0f, 0.0f}}, // bottom-right
            {{0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}}, // top-right
            {{0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}}, // top-right
            {{ -0.5f,  0.5f, -0.5f},  {0.0f, 1.0f}}, // top-left
            {{-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f}}, // bottom-left
            // front face
            {{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}}, // bottom-left
            {{ 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f}}, // top-right
            {{0.5f, -0.5f,  0.5f},  {1.0f, 0.0f}}, // bottom-right
            {{0.5f,  0.5f,  0.5f},  {1.0f, 1.0f}}, // top-right
            {{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}}, // bottom-left
            {{-0.5f,  0.5f,  0.5f},  {0.0f, 1.0f}}, // top-left
            // left face
            {{-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}}, // top-right
            {{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}}, // bottom-left
            {{-0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}}, // top-left
            {{ -0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}}, // bottom-left
            {{-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}}, // top-right
            {{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}}, // bottom-right
            // right face
            {{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f}}, // top-left
            {{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f}}, // top-right
            {{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}}, // bottom-right
            {{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}}, // bottom-right
            {{  0.5f, -0.5f,  0.5f}, { 0.0f, 0.0f}}, // bottom-left
            {{  0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f}}, // top-left
            // bottom face
            {{ -0.5f, -0.5f, -0.5f},  {0.0f, 1.0f}}, // top-right
            {{  0.5f, -0.5f,  0.5f},  {1.0f, 0.0f}}, // bottom-left
            {{  0.5f, -0.5f, -0.5f},  {1.0f, 1.0f}}, // top-left
            {{ 0.5f, -0.5f,  0.5f}, { 1.0f, 0.0f}}, // bottom-left
            {{ -0.5f, -0.5f, -0.5f}, { 0.0f, 1.0f}}, // top-right
            {{ -0.5f, -0.5f,  0.5f},  {0.0f, 0.0f}}, // bottom-right
            // top face
            {{ -0.5f,  0.5f, -0.5f},  {0.0f, 1.0f}}, // top-left
            {{ 0.5f,  0.5f, -0.5f}, { 1.0f, 1.0f}}, // top-right
            {{ 0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f}}, // bottom-right
            {{ 0.5f,  0.5f,  0.5f}, { 1.0f, 0.0f}}, // bottom-right
            {{ -0.5f,  0.5f,  0.5f}, { 0.0f, 0.0f}}, // bottom-left
            {{ -0.5f,  0.5f, -0.5f}, { 0.0f, 1.0f}}  // top-left
        };

        buffer = BufferBuilder::build(Buffer::Type::Array, vertices, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None);

        texture = TextureLoader::load(ASSETS_DIR "textures/triangle.jpg");

        vertex_array << std::pair<Vertex, Buffer&>(Vertex{}, *buffer);
    }

    void draw(const std::shared_ptr<ShaderProgram>& shader) const {
        vertex_array.bind();

        *shader << UniformSampler{"diffuse", texture};

        shader->use();

        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
};

bool done = false;

class Scene : public MouseMoveObserver, public KeyObserver {
public:
    ContextEventObserver context;
private:
    Camera camera;
    Cube cube;
    std::shared_ptr<ShaderProgram> shader;
    static constexpr glm::uvec2 window_size {500, 500};
public:
    Scene() : context{"Cubes demo", window_size, {{ WindowHint::Samples, 32 }}}, camera{window_size} {
        camera.setPosition({0.0f, 1.0f, -1.5f});
        context.makeCurrent();
        context.setWindowIcon(TextureLoader::loadGLFWImage(ASSETS_DIR "icons/demo.png"));
        context.setCursorMode(CursorMode::Disabled);

        context.enable(GL_DEPTH_TEST);
        context.enable(GL_CULL_FACE);
        context.setCullFace(GL_BACK);
        context.setFrontFace(GL_CW);

        context.registerObserver(static_cast<KeyObserver*>(this));
        context.registerObserver(static_cast<MouseMoveObserver *>(this));

        shader = MaterialCompiler::compile(SHADER_DIR "demo/cube");
    }

    void draw() const {
        *shader << UniformValue{"VP", camera.getProjection() * camera.getView()};

        cube.draw(shader);
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
    Scene scene;
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