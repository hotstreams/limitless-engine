#include "scene.hpp"
#include "assets.hpp"

#include <limitless/core/context.hpp>
#include <limitless/text/text_instance.hpp>
#include <limitless/renderer/color_picker.hpp>
#include <limitless/core/state_query.hpp>
#include <limitless/core/texture/state_texture.hpp>
#include <limitless/renderer/renderer.hpp>
#include <limitless/util/renderer_helper.hpp>

#include "limitless/core/profiler.hpp"
#include "limitless/logging/log.hpp"

namespace LimitlessMaterials {
    class MaterialsScene {
    private:
        glm::uvec2 window_size {1280, 720};

        Limitless::Context context;
        Limitless::Camera camera;
        Limitless::RendererSettings renderer_settings;
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
                        .cursor(Limitless::CursorMode::Disabled)
                        .swap_interval(0)
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
            , renderer_settings {Limitless::RendererSettings::builder()
//                .enable_height_fog()
                // A visible but not overwhelming default preset for terrain sample
                .height_fog_color(glm::vec3{0.65f, 0.75f, 0.85f})
                .height_fog_density(0.015f)
                .height_fog_height(0.0f)
                .height_fog_falloff(0.08f)
                .height_fog_start_distance(25.0f)
                .build()}
            , render {Limitless::Renderer::builder()
                .resolution(window_size)
                .settings(renderer_settings)
                .deferred()
                .build()}
            , assets {context, *render, ENGINE_ASSETS_DIR}
            , scene {context, assets} {
            camera.setPosition({125.01f, 35.85f, 254.0f});
            assets.recompileAssets(context, render->getSettings());
        }

        void onMouseMove(glm::dvec2 pos) {
            static glm::dvec2 last_move = {0, 0};

            auto offset = glm::vec2{pos.x - last_move.x, last_move.y - pos.y} * 0.5f;
            last_move = pos;

            camera.setRotation(
                camera.getPitch() + float(offset.y),
                camera.getYaw() + float(offset.x)
            );
        }

        void onKey(int key, [[maybe_unused]] int scancode, Limitless::InputState state, [[maybe_unused]] Limitless::Modifier modifier) {
            using namespace Limitless;
            if (key == GLFW_KEY_ESCAPE && state == Limitless::InputState::Pressed) {
                done = true;
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

            const auto speed = 10.0f;

            if (context.isPressed(GLFW_KEY_W)) {
                camera.setPosition(camera.getPosition() + camera.getFront() * delta * speed);
            }

            if (context.isPressed(GLFW_KEY_S)) {
                camera.setPosition(camera.getPosition() - camera.getFront() * delta * speed);
            }

            if (context.isPressed(GLFW_KEY_A)) {
                camera.setPosition(camera.getPosition() - camera.getRight() * delta * speed);
            }

            if (context.isPressed(GLFW_KEY_D)) {
                camera.setPosition(camera.getPosition() + camera.getRight() * delta * speed);
            }

            if (context.isPressed(GLFW_KEY_SPACE)) {
                camera.setPosition(camera.getPosition() + camera.getUp() * delta * speed);
            }

            if (context.isPressed(GLFW_KEY_Z)) {
                camera.setPosition(camera.getPosition() - camera.getUp() * delta * speed);
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

template<typename ProfilerType>
static void printProfilerFrames(const ProfilerType& profiler) {
    for (const auto& [name, frame] : profiler.frames) {
        std::cout << name << ": min " << std::chrono::duration_cast<std::chrono::microseconds>(frame.getMinDuration()).count()
                  << ", max " << std::chrono::duration_cast<std::chrono::microseconds>(frame.getMaxDuration()).count()
                  << ", avg " << std::chrono::duration_cast<std::chrono::microseconds>(frame.getAverageDuration()).count()
                  << ", n " << frame.getCount() << std::endl;
    }
}

int main() {
    // Initialize logging system
    Limitless::Log::init();

    LimitlessMaterials::MaterialsScene scene;
    scene.gameLoop();

    std::cout << "GPU" << std::endl;
    printProfilerFrames(Limitless::global_gpu_profiler);
    std::cout << "CPU" << std::endl;
    printProfilerFrames(Limitless::global_cpu_profiler);

    return 0;
}
