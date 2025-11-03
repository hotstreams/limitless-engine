#include <iostream>

#include "scene.hpp"
#include "assets.hpp"

#include <limitless/core/context.hpp>
#include <limitless/text/text_instance.hpp>
#include <limitless/renderer/color_picker.hpp>
#include <limitless/core/state_query.hpp>
#include <limitless/core/texture/state_texture.hpp>
#include <limitless/renderer/renderer.hpp>

namespace LimitlessMaterials {
    class MaterialsScene {
    private:
        glm::uvec2 window_size {1080, 720};

        Limitless::Context context;
        Limitless::Camera camera;
        std::unique_ptr<Limitless::Renderer> render;
        Assets assets;
        Scene scene;

        bool done{};
        bool hidden_text{};
    public:
        MaterialsScene()
            : context {
                Limitless::Context::builder()
                    .title("material-demo")
                    .size(window_size)
//                    .not_resizeable()
                    .cursor(Limitless::CursorMode::Normal)
                    .swap_interval(1)
                    .sticky_keys()
                    .on_framebuffer_change([&] (glm::uvec2 size) {
                        onFramebufferChange(size);
                    })
                    .on_mouse_move([&](glm::dvec2 pos) {
                        onMouseMove(pos);
                    })
                    .on_key_press([&](int key, int scancode, Limitless::InputState state, Limitless::Modifier modifier) {
                        onKey(key, scancode, state, modifier);
                    })
                    .build()
             }
            , camera {window_size}
            , render {Limitless::Renderer::builder()
                        .resolution(window_size)
                        .deferred()
                        .build()}
            , assets {context, *render, ENGINE_ASSETS_DIR}
            , scene {context, assets} {
            camera.setPosition({-3.0f, 2.0f, 3.0f});

            assets.recompileAssets(context, render->getSettings());
        }

        void onMouseMove(glm::dvec2 pos) {
            static glm::dvec2 last_move = {0, 0};

            auto offset = glm::vec2{pos.x - last_move.x, last_move.y - pos.y};
            last_move = pos;

            camera.mouseMove(offset);
        }

        void onKey(int key, [[maybe_unused]] int scancode, Limitless::InputState state, [[maybe_unused]] Limitless::Modifier modifier) {
            using namespace Limitless;
            if (key == GLFW_KEY_ESCAPE && state == Limitless::InputState::Pressed) {
                done = true;
            }

            if (key == GLFW_KEY_SPACE && state == Limitless::InputState::Pressed) {
                camera.getMoveSpeed() *= 5.0f;
            }

            if (key == GLFW_KEY_SPACE && state == Limitless::InputState::Released) {
                camera.getMoveSpeed() /= 5.0f;
            }

            if (key == GLFW_KEY_GRAVE_ACCENT && state == Limitless::InputState::Released) {
                hidden_text = !hidden_text;
            }
        }

        void onFramebufferChange(glm::uvec2 size) {
            camera.updateProjection(size);
            window_size = size;
            render->onFramebufferChange(size);
        }

        void handleInput(float delta) noexcept {
            using namespace Limitless;

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
                auto current_time = steady_clock::now();
                static auto last_time = steady_clock::now();
                auto delta_time = duration_cast<duration<float>>(current_time - last_time).count();
                last_time = current_time;

                scene.update(context, camera);
                render->render(context, assets, scene.getScene(), camera);

                context.swapBuffers();
                context.pollEvents();

                handleInput(delta_time);
            }
        }
    };
}

static void terminationHandler()
{
    for ( int i = 0 ; i < 1000; i++)
    {
        std::cout << i << std::endl;
    }
}

int main() {
    // std::set_terminate(terminationHandler);

    LimitlessMaterials::MaterialsScene scene;
    scene.gameLoop();

    return 0;
}
