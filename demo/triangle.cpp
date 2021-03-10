#include <core/context_observer.hpp>
#include <core/shader_compiler.hpp>
#include <texture_loader.hpp>
#include <core/buffer_builder.hpp>
#include <core/vertex_array.hpp>
#include <material_system/material_compiler.hpp>
#include <core/uniform.hpp>
#include <core/shader_program.hpp>

#define ASSETS_DIR "../assets/"

using namespace LimitlessEngine;

class Triangle {
private:
    VertexArray vertex_array;
    std::unique_ptr<Buffer> buffer;
    std::shared_ptr<ShaderProgram> shader;
    std::shared_ptr<Texture> texture;
public:
    Triangle() noexcept {
        std::vector<Vertex> vertices = {   {  glm::vec3(-1.0, -1.0, 0.0), glm::vec2(0.0, 0.0) },
                                           {  glm::vec3(0.0, 1.0, 0.0),   glm::vec2(0.5, 1.0) },
                                           {  glm::vec3(1.0, -1.0, 0.0),  glm::vec2(1.0, 0.0) } };
        buffer = BufferBuilder::build(Buffer::Type::Array, vertices, Buffer::Usage::StaticDraw, Buffer::MutableAccess::None);

        ShaderCompiler compiler;
        shader = compiler.compile(SHADER_DIR "demo/triangle");

        texture = TextureLoader::load(ASSETS_DIR "textures/triangle.jpg");

        vertex_array << std::pair<Vertex, Buffer&>(Vertex{}, *buffer);
    }

    void draw() const {
        vertex_array.bind();

        *shader << UniformValue{"triangle_color", glm::vec4{1.0f, 0.5f, 0.25f, 1.0f}}
                << UniformSampler{"diffuse", texture};

        shader->use();

        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
};

int main() {
    ContextEventObserver context = ContextEventObserver("Triangle demo", { 500, 500 }, {{ WindowHint::Samples, 32 }});
    context.makeCurrent();
    context.setWindowIcon(TextureLoader::loadGLFWImage(ASSETS_DIR "icons/demo.png"));

    Triangle triangle;

    while (!context.shouldClose()) {
        context.clear(Clear::ColorDepth);
        context.setViewPort(context.getSize());

        triangle.draw();

        context.pollEvents();
        context.swapBuffers();
    }

    return 0;
}